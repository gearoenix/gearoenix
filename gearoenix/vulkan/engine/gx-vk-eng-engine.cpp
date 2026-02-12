#include "gx-vk-eng-engine.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/gx-cr-profiler.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../buffer/gx-vk-buf-uniform.hpp"
#include "../camera/gx-vk-cmr-manager.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-imgui-manager.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../image/gx-vk-img-manager.hpp"
#include "../light/gx-vk-lt-manager.hpp"
#include "../material/gx-vk-mat-manager.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../mesh/gx-vk-msh-manager.hpp"
#include "../model/gx-vk-mdl-manager.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../reflection/gx-vk-rfl-manager.hpp"
#include "../sampler/gx-vk-smp-manager.hpp"
#include "../scene/gx-vk-scn-manager.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"
#include "../texture/gx-vk-txt-manager.hpp"
#include "gx-vk-eng-frame.hpp"

void gearoenix::vulkan::engine::Engine::initialize_frame()
{
    for (int frame_index = 0; frame_index < frames_count; ++frame_index) {
        auto& swapchain_frame = swapchain->get_frames()[frame_index];
        frames[frame_index] = std::make_unique<Frame>(
            std::shared_ptr(swapchain_frame.view),
            std::shared_ptr(swapchain_frame.imgui_view),
            frame_index);
    }
}

void gearoenix::vulkan::engine::Engine::window_resized()
{
    GX_UNIMPLEMENTED;
}

gearoenix::vulkan::engine::Engine::Engine()
    : render::engine::Engine(render::engine::Type::Vulkan)
    , Singleton<Engine>(this)
    , instance(Instance::construct())
    , surface(new Surface())
    , physical_device(new device::Physical())
    , logical_device(new device::Logical())
    , swapchain(new Swapchain())
    , memory_manager(new memory::Manager())
    , command_manager(new command::Manager())
    , buffer_manager(new buffer::Manager())
    , render_queue(new queue::Queue())
    , image_manager(new image::Manager())
    , sampler_manager(new sampler::Manager())
    , vk_texture_manager(new texture::Manager())
    , vk_mesh_manager(new mesh::Manager())
    , vk_material_manager(new material::Manager())
    , vk_model_manager(new model::Manager())
    , vk_light_manager(new light::Manager())
    , vk_camera_manager(new camera::Manager())
    , vk_scene_manager(new scene::Manager())
    , vk_reflection_manager(new reflection::Manager())
{
    frames_count = frames_in_flight;
    half_depth_clip = true;
    mesh_manager = std::unique_ptr<render::mesh::Manager>(vk_mesh_manager);
    texture_manager = std::unique_ptr<render::texture::Manager>(vk_texture_manager);
    material_manager = std::unique_ptr<material::Manager>(vk_material_manager);
    model_manager = std::unique_ptr<render::model::Manager>(vk_model_manager);
    camera_manager = std::unique_ptr<camera::Manager>(vk_camera_manager);
    light_manager = std::unique_ptr<light::Manager>(vk_light_manager);
    scene_manager = std::unique_ptr<scene::Manager>(vk_scene_manager);
    reflection_manager = std::unique_ptr<reflection::Manager>(vk_reflection_manager);

    bindless_descriptor_manager = std::make_unique<descriptor::Bindless>(
        vk_scene_manager->get_uniform_indexer().get_gpu_buffer(),
        vk_camera_manager->get_camera_uniform_indexer().get_gpu_buffer(),
        vk_model_manager->get_model_uniform_indexer().get_gpu_buffer(),
        vk_material_manager->get_uniform_indexer().get_gpu_buffer(),
        vk_light_manager->get_points_uniform_indexer().get_gpu_buffer(),
        vk_light_manager->get_directionals_uniform_indexer().get_gpu_buffer(),
        vk_light_manager->get_directional_shadow_casters_uniform_indexer().get_gpu_buffer(),
        vk_model_manager->get_bone_uniform_indexer().get_gpu_buffer(),
        vk_reflection_manager->get_uniform_indexer().get_gpu_buffer(),
        vk_camera_manager->get_cameras_joint_models_uniform_indexer().get_gpu_buffer());
    pipeline_manager = std::make_unique<pipeline::Manager>();
    imgui_manager = std::make_unique<ImGuiManager>();
    initialize_frame();
}

gearoenix::vulkan::engine::Engine::~Engine()
{
    core::job::execute_current_thread_jobs();
    logical_device->wait_to_finish();
    world = nullptr;
    sampler_manager = nullptr;
    buffer_manager = nullptr;
    render_queue = nullptr;
    mesh_manager = nullptr;
    texture_manager = nullptr;
    material_manager = nullptr;
    model_manager = nullptr;
    camera_manager = nullptr;
    light_manager = nullptr;
    scene_manager = nullptr;
    reflection_manager = nullptr;
    bindless_descriptor_manager = nullptr;
    imgui_manager = nullptr;
    logical_device->wait_to_finish();
    frames = {};
}

void gearoenix::vulkan::engine::Engine::start_frame()
{
    core::job::execute_current_thread_jobs();

    render::engine::Engine::start_frame();

    core::job::execute_current_thread_jobs();

    if (swapchain->get_is_valid()) {
        GX_PROFILE_BEGIN(vulkan-start-frame-fence-wait);
        auto& fence = *frames[frame_number]->render_fence;
        fence.wait();
        fence.reset();
        GX_PROFILE_END(vulkan-start-frame-fence-wait);
    } else if (!platform::BaseApplication::get().get_window_resizing()) {
        logical_device->wait_to_finish();
        frames = {};
        // depth_stencil = image::View::create_depth_stencil();
        swapchain->initialize();
        initialize_frame();
    }

    core::job::execute_current_thread_jobs();

    imgui_manager->start_frame();

    core::job::execute_current_thread_jobs();

    GX_PROFILE_BEGIN(vulkan-start-frame-swapchain-wait);
    if (swapchain->get_is_valid()) {
        swapchain->acquire_next_image(*frames[frame_number]->present_semaphore);
    }
    GX_PROFILE_END(vulkan-start-frame-swapchain-wait);
}

void gearoenix::vulkan::engine::Engine::end_frame()
{
    render::engine::Engine::end_frame();

    GX_PROFILE_BEGIN(vulkan-end-frame);
    imgui_manager->end_frame();
    if (swapchain->get_is_valid()) {
        submit();
        swapchain->present();
    }
    GX_PROFILE_END(vulkan-end-frame);

    core::job::execute_current_thread_jobs();
}

void gearoenix::vulkan::engine::Engine::upload_imgui_fonts()
{
    imgui_manager->upload_fonts();
}

void gearoenix::vulkan::engine::Engine::submit()
{
    const auto& frame = *frames[frame_number];
    auto& cmd = *frame.cmd;
    const auto vk_cmd = cmd.get_vulkan_data();
    cmd.begin();
    upload_uniforms();
    buffer_manager->upload_dynamics(vk_cmd);
    bindless_descriptor_manager->bind(vk_cmd);
    vk_scene_manager->submit(vk_cmd);
    imgui_manager->update();
    cmd.end();

    constexpr VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    render_queue->submit(
        1, frame.present_semaphore->get_vulkan_data_ptr(),
        &wait_stage,
        1, cmd.get_vulkan_data_ptr(),
        1, swapchain->get_present_semaphore().get_vulkan_data_ptr(),
        frame.render_fence->get_vulkan_data());
}

gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame()
{
    return *frames[frame_number];
}

const gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame() const
{
    return *frames[frame_number];
}

bool gearoenix::vulkan::engine::Engine::is_supported()
{
    if (!Loader::load()) {
        return false;
    }

    const auto instance_result = Instance::construct();
    if (!instance_result) {
        return false;
    }

    const auto& instance = *instance_result;
    const auto gpus = device::Physical::get_available_devices(instance.get_vulkan_data());
    return !gpus.empty();
}

void gearoenix::vulkan::engine::Engine::flush()
{
    render::engine::Engine::flush();
    logical_device->wait_to_finish();
}

void gearoenix::vulkan::engine::Engine::upload_uniforms()
{
    vk_camera_manager->upload_uniforms();
    vk_light_manager->upload_uniforms();
    vk_material_manager->upload_uniforms();
    vk_model_manager->upload_uniforms();
    vk_scene_manager->upload_uniforms();
    vk_reflection_manager->upload_uniforms();
}
#endif
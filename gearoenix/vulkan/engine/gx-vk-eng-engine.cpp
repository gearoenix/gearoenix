#include "gx-vk-eng-engine.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
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
#include "../scene/gx-vk-scn-manager.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"
#include "../texture/gx-vk-txt-manager.hpp"
#include "gx-vk-eng-frame.hpp"

void gearoenix::vulkan::engine::Engine::initialize_frame()
{
    frames_count = static_cast<decltype(frames_count)>(swapchain->get_image_views().size());
    for (auto frame_index = decltype(frames_count) { 0 }; frame_index < frames_count; ++frame_index) {
        frames[frame_index] = std::make_unique<Frame>(std::shared_ptr(swapchain->get_image_views()[frame_index]));
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
    , pipeline_manager(new pipeline::Manager())
    , buffer_manager(new buffer::Manager())
    , imgui_manager(new ImGuiManager())
    , image_manager(new image::Manager())
    , vk_texture_manager(new texture::Manager())
    , vk_mesh_manager(new mesh::Manager())
    , vk_material_manager(new material::Manager())
    , vk_model_manager(new model::Manager())
    , vk_light_manager(new light::Manager())
    , vk_camera_manager(new camera::Manager())
    , vk_scene_manager(new scene::Manager())
    , render_queue(new queue::Queue())
{
    frames_count = swapchain->get_image_views().size();
    mesh_manager = std::unique_ptr<render::mesh::Manager>(vk_mesh_manager);
    texture_manager = std::unique_ptr<render::texture::Manager>(vk_texture_manager);
    material_manager = std::unique_ptr<material::Manager>(vk_material_manager);
    model_manager = std::unique_ptr<render::model::Manager>(vk_model_manager);
    camera_manager = std::unique_ptr<camera::Manager>(vk_camera_manager);
    light_manager = std::unique_ptr<light::Manager>(vk_light_manager);
    scene_manager = std::unique_ptr<scene::Manager>(vk_scene_manager);
    reflection_manager = std::make_unique<reflection::Manager>();

    bindless_descriptor_manager = std::make_unique<descriptor::Bindless>(
        *scene::Manager::get_uniform_buffer().get_gpu(),
        *camera::Manager::get_uniform_buffer().get_gpu(),
        *model::Manager::get_uniform_buffer().get_gpu(),
        *material::Manager::get_uniform_buffer().get_gpu(),
        *light::Manager::get_uniform_buffer().get_gpu());
    initialize_frame();
}

gearoenix::vulkan::engine::Engine::~Engine()
{
    world = nullptr;
    bindless_descriptor_manager = nullptr;
    logical_device->wait_to_finish();
    imgui_manager = nullptr;
    frames = {};
}

void gearoenix::vulkan::engine::Engine::start_frame()
{
    render::engine::Engine::start_frame();
    if (swapchain_image_is_valid) {
        frames[frame_number]->render_fence->wait();
    } else if (!platform::BaseApplication::get().get_window_resizing()) {
        logical_device->wait_to_finish();
        frames = {};
        // depth_stencil = image::View::create_depth_stencil();
        swapchain->initialize();
        initialize_frame();
        swapchain_image_is_valid = true;
        swapchain_image_index = 0;
    }
    imgui_manager->start_frame();
    if (swapchain_image_is_valid) {
        const auto& frame_data = *frames[frame_number];
        if (const auto next_image = swapchain->get_next_image_index(*frame_data.present_semaphore); next_image.has_value()) {
            swapchain_image_index = *next_image;
            frame_data.render_fence->reset();
        } else {
            swapchain_image_is_valid = false;
        }
    }
}

void gearoenix::vulkan::engine::Engine::end_frame()
{
    render::engine::Engine::end_frame();
    imgui_manager->end_frame();
    if (swapchain_image_is_valid) {
        vk_mesh_manager->update();
        submit();
        swapchain_image_is_valid = present();
    }
}

void gearoenix::vulkan::engine::Engine::upload_imgui_fonts()
{
    imgui_manager->upload_fonts();
}

bool gearoenix::vulkan::engine::Engine::present()
{
    VkPresentInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = frames[frame_number]->end_semaphore->get_vulkan_data_ptr();
    info.swapchainCount = 1;
    info.pSwapchains = Swapchain::get().get_vulkan_data_ptr();
    info.pImageIndices = &swapchain_image_index;
    const auto present_result = vkQueuePresentKHR(render_queue->get_vulkan_data(), &info);
    if (VK_ERROR_OUT_OF_DATE_KHR == present_result) {
        return false;
    }
    if (VK_SUCCESS != present_result) {
        GX_LOG_F("Presentation failed with result: " << result_to_string(present_result));
    }
    return true;
}

void gearoenix::vulkan::engine::Engine::submit()
{
    auto& frame = *frames[frame_number];
    auto& cmd = *frame.cmd;
    const auto vk_cmd = cmd.get_vulkan_data();
    cmd.begin();
    // TODO: update all the uniforms
    vk_scene_manager->update_uniforms();
    buffer_manager->upload_dynamics(vk_cmd);
    bindless_descriptor_manager->bind(vk_cmd);
    vk_scene_manager->submit(vk_cmd);
    imgui_manager->update();
    cmd.end();
    render_queue->submit(cmd, *frame.render_fence); // TODO: make sure it is the way, maybe I need to call the other function.
}

gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame()
{
    return *frames[swapchain_image_index];
}

const gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame() const
{
    return *frames[swapchain_image_index];
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
#endif
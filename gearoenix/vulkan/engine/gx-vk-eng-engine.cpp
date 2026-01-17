#include "gx-vk-eng-engine.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../camera/gx-vk-cmr-manager.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-imgui-manager.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../mesh/gx-vk-msh-manager.hpp"
#include "../queue/gx-vk-qu-graph.hpp"
#include "../reflection/gx-vk-rfl-manager.hpp"
#include "../descriptor/gx-vk-des-manager.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-eng-frame.hpp"
#include "../image/gx-vk-img-manager.hpp"
#include "../texture/gx-vk-txt-manager.hpp"
#include "../command/gx-vk-cmd-manager.hpp"

void gearoenix::vulkan::engine::Engine::initialize_frame()
{
    frames_count = static_cast<decltype(frames_count)>(swapchain->get_image_views().size());
    frames.reserve(static_cast<std::uint64_t>(frames_count));
    for (auto frame_index = decltype(frames_count) { 0 }; frame_index < frames_count; ++frame_index) {
        frames.emplace_back(new Frame(swapchain->get_image_views()[frame_index], depth_stencil, render_pass));
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
    , descriptor_manager(new descriptor::Manager())
    , pipeline_manager()
    , buffer_manager()
    , depth_stencil(image::View::create_depth_stencil())
    , render_pass()
    , graphed_queue(new queue::Graph())
    , imgui_manager(new ImGuiManager())
    , vk_image_manager(new image::Manager())
    , vk_texture_manager(new texture::Manager())
    , vk_mesh_manager(new mesh::Manager())
{
    frames_count = swapchain->get_image_views().size();
    mesh_manager = std::unique_ptr<render::mesh::Manager>(vk_mesh_manager);
    texture_manager = std::unique_ptr<render::texture::Manager>(vk_texture_manager);
    camera_manager = std::make_unique<camera::Manager>();
    reflection_manager = std::make_unique<reflection::Manager>();
    initialize_frame();
}

gearoenix::vulkan::engine::Engine::~Engine()
{
    world = nullptr;
    bindless_descriptor_manager = nullptr;
    logical_device->wait_to_finish();
    imgui_manager = nullptr;
    frames.clear();
}

void gearoenix::vulkan::engine::Engine::start_frame()
{
    render::engine::Engine::start_frame();
    if (!swapchain_image_is_valid && !platform::BaseApplication::get().get_window_resizing()) {
        logical_device->wait_to_finish();
        frames.clear();
        depth_stencil = image::View::create_depth_stencil();
        swapchain->initialize();
        initialize_frame();
        swapchain_image_is_valid = true;
        swapchain_image_index = 0;
    }
    imgui_manager->start_frame();
    if (swapchain_image_is_valid) {
        if (const auto next_image = swapchain->get_next_image_index(graphed_queue->get_present_semaphore()); next_image.has_value()) {
            swapchain_image_index = *next_image;
            graphed_queue->start_frame();
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
        imgui_manager->update();
        swapchain_image_is_valid = graphed_queue->present(swapchain_image_index);
    }
}

void gearoenix::vulkan::engine::Engine::upload_imgui_fonts()
{
    imgui_manager->upload_fonts();
}

gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame()
{
    return *frames[swapchain_image_index];
}

const gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame() const
{
    return *frames[swapchain_image_index];
}

const gearoenix::vulkan::Framebuffer& gearoenix::vulkan::engine::Engine::get_current_framebuffer() const
{
    return *frames[swapchain_image_index]->framebuffer;
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
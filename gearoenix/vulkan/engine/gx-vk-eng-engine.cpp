#include "gx-vk-eng-engine.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../camera/gx-vk-cmr-manager.hpp"
#include "../gx-vk-imgui-manager.hpp"
#include "../mesh/gx-vk-msh-manager.hpp"
#include "../queue/gx-vk-qu-graph.hpp"
#include "../reflection/gx-vk-rfl-manager.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-eng-frame.hpp"

void gearoenix::vulkan::engine::Engine::initialize_frame()
{
    frames_count = static_cast<decltype(frames_count)>(swapchain.get_image_views().size());
    frames.reserve(static_cast<std::uint64_t>(frames_count));
    for (auto frame_index = decltype(frames_count) { 0 }; frame_index < frames_count; ++frame_index) {
        frames.emplace_back(new Frame(swapchain, depth_stencil, render_pass, frame_index));
    }
}

void gearoenix::vulkan::engine::Engine::window_resized()
{
    GX_UNIMPLEMENTED;
}

gearoenix::vulkan::engine::Engine::Engine(platform::Application& platform_application)
    : render::engine::Engine(render::engine::Type::Vulkan, platform_application)
    , instance(*Instance::construct(&platform_application))
    , surface(instance, platform_application)
    , physical_device(surface)
    , logical_device(physical_device)
    , swapchain(*this)
    , memory_manager(*this)
    , command_manager(*this)
    , descriptor_manager(logical_device)
    , pipeline_manager(*this)
    , buffer_manager(memory_manager, *this)
    , depth_stencil(image::View::create_depth_stencil(memory_manager))
    , render_pass(swapchain)
    , graphed_queue(new queue::Graph(*this))
    , imgui_manager(new ImGuiManager(*this))
    , vulkan_mesh_manager(new mesh::Manager(*this))
{
    mesh_manager = std::unique_ptr<render::mesh::Manager>(vulkan_mesh_manager);
    camera_manager = std::make_unique<camera::Manager>(*this);
    reflection_manager = std::make_unique<reflection::Manager>(*this);
    initialize_frame();
}

gearoenix::vulkan::engine::Engine::~Engine()
{
    world = nullptr;
    logical_device.wait_to_finish();
    imgui_manager = nullptr;
    frames.clear();
}

void gearoenix::vulkan::engine::Engine::start_frame()
{
    render::engine::Engine::start_frame();
    if (!swapchain_image_is_valid && !platform_application.get_base().get_window_resizing()) {
        logical_device.wait_to_finish();
        frames.clear();
        depth_stencil = image::View::create_depth_stencil(memory_manager);
        swapchain.initialize();
        initialize_frame();
        swapchain_image_is_valid = true;
        swapchain_image_index = 0;
    }
    imgui_manager->start_frame();
    if (swapchain_image_is_valid) {
        const auto next_image = swapchain.get_next_image_index(graphed_queue->get_present_semaphore());
        if (next_image.has_value()) {
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
        vulkan_mesh_manager->update();
        imgui_manager->update();
        swapchain_image_is_valid = graphed_queue->present(swapchain, swapchain_image_index);
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
    if (!Loader::load())
        return false;
    auto instance_result = Instance::construct(nullptr);
    if (!instance_result.has_value())
        return false;
    auto& instance = instance_result.value();
    const auto gpus = device::Physical::get_available_devices(instance.get_vulkan_data());
    return !gpus.empty();
}

#endif

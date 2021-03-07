#include "gx-vk-eng-engine.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-imgui-manager.hpp"
#include "../mesh/gx-vk-msh-manager.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-eng-frame.hpp"

void gearoenix::vulkan::engine::Engine::initialize_frame() noexcept
{
    frames_count = static_cast<decltype(frames_count)>(swapchain.get_image_views().size());
    frames.reserve(static_cast<std::size_t>(frames_count));
    for (auto frame_index = decltype(frames_count) { 0 }; frame_index < frames_count; ++frame_index) {
        frames.emplace_back(new Frame(swapchain, depth_stencil, render_pass, frame_index));
    }
}

gearoenix::vulkan::engine::Engine::Engine(const platform::Application& platform_application) noexcept
    : render::engine::Engine(render::engine::Type::Vulkan, platform_application)
    , instance(*Instance::construct(platform_application.get_base().get_configuration().get_application_name().c_str()))
    , surface(instance, platform_application)
    , physical_device(surface)
    , logical_device(physical_device)
    , swapchain(logical_device)
    , memory_manager(*this)
    , command_manager(logical_device)
    , descriptor_manager(logical_device)
    , pipeline_manager(logical_device)
    , buffer_manager(memory_manager, *this)
    , mesh_manager(mesh::Manager::construct(*this))
    , depth_stencil(image::View::create_depth_stencil(memory_manager))
    , render_pass(swapchain)
    , graphic_queue(new queue::Queue(*this))
    , imgui_manager(new ImGuiManager(*this))
{
    initialize_frame();
}

gearoenix::vulkan::engine::Engine::~Engine() noexcept
{
    world = nullptr;
    logical_device.wait_to_finish();
    imgui_manager = nullptr;
    frames.clear();
}

void gearoenix::vulkan::engine::Engine::start_frame() noexcept
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
        const auto next_image = swapchain.get_next_image_index(graphic_queue->get_present_semaphore());
        if (next_image.has_value()) {
            swapchain_image_index = *next_image;
            graphic_queue->start_frame();
        } else {
            swapchain_image_is_valid = false;
        }
    }
}

void gearoenix::vulkan::engine::Engine::end_frame() noexcept
{
    render::engine::Engine::end_frame();
    imgui_manager->end_frame();
    if (swapchain_image_is_valid) {
        mesh_manager->update();
        imgui_manager->update();
        swapchain_image_is_valid = graphic_queue->present(swapchain, swapchain_image_index);
    }
}

void gearoenix::vulkan::engine::Engine::upload_imgui_fonts() noexcept
{
    imgui_manager->upload_fonts();
}

void gearoenix::vulkan::engine::Engine::create_mesh(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    mesh_manager->create(name, vertices, indices, c);
}

gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame() noexcept
{
    return *frames[swapchain_image_index];
}

const gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame() const noexcept
{
    return *frames[swapchain_image_index];
}

const gearoenix::vulkan::Framebuffer& gearoenix::vulkan::engine::Engine::get_current_framebuffer() const noexcept
{
    return *frames[swapchain_image_index]->framebuffer;
}

bool gearoenix::vulkan::engine::Engine::is_supported() noexcept
{
    if (!Loader::load())
        return false;
    auto instance_result = Instance::construct(GX_APPLICATION_NAME);
    if (!instance_result.has_value())
        return false;
    auto& instance = instance_result.value();
    const auto gpus = device::Physical::get_available_devices(instance.get_vulkan_data());
    return !gpus.empty();
}

#endif

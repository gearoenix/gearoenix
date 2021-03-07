#include "gx-vk-eng-engine.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../gx-vk-check.hpp"
#include "../mesh/gx-vk-msh-manager.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-eng-frame.hpp"
#include <imgui_impl_vulkan.h>

void gearoenix::vulkan::engine::Engine::setup_imgui() noexcept
{
    ImGui_ImplVulkan_InitInfo info {};
    info.Instance = instance.get_vulkan_data();
    info.PhysicalDevice = physical_device.get_vulkan_data();
    info.Device = logical_device.get_vulkan_data();
    info.QueueFamily = physical_device.get_graphics_queue_node_index();
    info.Queue = graphic_queue->get_vulkan_data();
    info.PipelineCache = pipeline_manager.get_cache().get_vulkan_data();
    info.DescriptorPool = descriptor_manager.get_imgui().get_vulkan_data();
    info.MinImageCount = static_cast<decltype(info.MinImageCount)>(swapchain.get_image_views().size());
    info.ImageCount = info.MinImageCount;
#ifdef GX_DEBUG_MODE
    info.CheckVkResultFn = +[](const VkResult result) noexcept {
        GX_VK_CHK(result)
    };
#endif
    ImGui_ImplVulkan_LoadFunctions(+[](const char* const name, void*) noexcept {
        return Loader::get(name);
    });
    ImGui_ImplVulkan_Init(&info, render_pass.get_vulkan_data());
    vulkan_upload_imgui_fonts();
}

void gearoenix::vulkan::engine::Engine::vulkan_upload_imgui_fonts() noexcept
{
    command::Buffer cmd = command_manager.create(command::Type::Primary);
    cmd.begin();
    ImGui_ImplVulkan_CreateFontsTexture(cmd.get_vulkan_data());
    cmd.end();
    sync::Fence fence(logical_device);
    graphic_queue->submit(cmd, fence);
    fence.wait();
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void gearoenix::vulkan::engine::Engine::start_frame_imgui() noexcept
{
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();
    (void)this;
}

void gearoenix::vulkan::engine::Engine::initialize_frame() noexcept
{
    frames_count = static_cast<decltype(frames_count)>(swapchain.get_image_views().size());
    frames.reserve(static_cast<std::size_t>(frames_count));
    for (auto frame_index = decltype(frames_count) { 0 }; frame_index < frames_count; ++frame_index) {
        frames.emplace_back(new Frame(command_manager, swapchain, depth_stencil, render_pass, frame_index));
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
{
    initialize_frame();
    setup_imgui();
}

gearoenix::vulkan::engine::Engine::~Engine() noexcept
{
    world = nullptr;
    logical_device.wait_to_finish();
    ImGui_ImplVulkan_Shutdown();
    ImGui::DestroyContext();
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
    start_frame_imgui();
    if (swapchain_image_is_valid) {
        const auto next_image = swapchain.get_next_image_index(*frames[frame_number]->present_complete);
        if (next_image.has_value()) {
            swapchain_image_index = *next_image;
        } else {
            swapchain_image_is_valid = false;
        }
        if (swapchain_image_is_valid) {
            //            frames[swapchain_image_index]->begin();
            frames[frame_number]->begin();
        }
    }
}

void gearoenix::vulkan::engine::Engine::end_frame() noexcept
{
    render::engine::Engine::end_frame();
    // ImGui
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();

    if (swapchain_image_is_valid) {
        mesh_manager->update();

        auto& frame = *frames[swapchain_image_index];
        auto& sync_frame = *frames[frame_number];
        auto& cmd = *sync_frame.draw_command;
        auto& render_complete = *sync_frame.render_complete;
        auto& gqu = *graphic_queue;
        cmd.begin(render_pass, *frame.framebuffer);
        ImGui_ImplVulkan_RenderDrawData(draw_data, cmd.get_vulkan_data());
        cmd.end_render_pass();
        cmd.end();
        gqu.submit(
            *sync_frame.present_complete, cmd, render_complete, *sync_frame.draw_wait,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        swapchain_image_is_valid = gqu.present(render_complete, swapchain, swapchain_image_index);
    }
}

void gearoenix::vulkan::engine::Engine::upload_imgui_fonts() noexcept
{
    vulkan_upload_imgui_fonts();
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

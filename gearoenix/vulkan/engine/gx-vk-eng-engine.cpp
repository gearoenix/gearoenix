#include "gx-vk-eng-engine.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../gx-vk-check.hpp"
#include <imgui_impl_vulkan.h>

void gearoenix::vulkan::engine::Engine::setup_imgui() noexcept
{
    ImGui_ImplVulkan_InitInfo info {};
    info.Instance = instance.get_vulkan_data();
    info.PhysicalDevice = physical_device.get_vulkan_data();
    info.Device = logical_device.get_vulkan_data();
    info.QueueFamily = physical_device.get_graphics_queue_node_index();
    info.Queue = logical_device.get_graphic_queue();
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
    command::Buffer command_buffer = command_manager.create(command::Type::Primary);
    command_buffer.begin();
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer.get_vulkan_data());
    VkSubmitInfo end_info = {};
    end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    end_info.commandBufferCount = 1;
    end_info.pCommandBuffers = command_buffer.get_vulkan_data_ptr();
    command_buffer.end();
    GX_VK_CHK(vkQueueSubmit(logical_device.get_graphic_queue(), 1, &end_info, nullptr))
    logical_device.wait_to_finish();
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
        frames.emplace_back(command_manager, swapchain, depth_stencil, render_pass, frame_index);
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
    , mesh_manager(*this)
    , depth_stencil(image::View::create_depth_stencil(memory_manager))
    , render_pass(swapchain)
{
    initialize_frame();
    setup_imgui();
}

gearoenix::vulkan::engine::Engine::~Engine() noexcept
{
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
        const auto next_image = swapchain.get_next_image_index(frames[frame_number].present_complete);
        if (next_image.has_value()) {
            swapchain_image_index = *next_image;
        } else {
            swapchain_image_is_valid = false;
        }
        if (swapchain_image_is_valid) {
            frames[swapchain_image_index].begin();
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
        auto& frame = frames[swapchain_image_index];
        buffer_manager.do_copies(frame.copy_command);
        GX_TODO // TODO
            // mesh_manager.create_accelerators();
            // Record ImGui commands
            frame.draw_command.begin(render_pass, frame.framebuffer);
        ImGui_ImplVulkan_RenderDrawData(draw_data, frame.draw_command.get_vulkan_data());
        frame.draw_command.end_render_pass();
        frame.draw_command.end();
        // Submitting
        const VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submit_info;
        GX_SET_ZERO(submit_info)
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = frames[frame_number].present_complete.get_vulkan_data_ptr();
        submit_info.pWaitDstStageMask = &wait_stage;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = frame.draw_command.get_vulkan_data_ptr();
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = frames[frame_number].render_complete.get_vulkan_data_ptr();
        GX_VK_CHK(
            vkQueueSubmit(logical_device.get_graphic_queue(), 1, &submit_info, frame.draw_wait.get_vulkan_data()))
        // Presenting
        const auto present_image_index = static_cast<std::uint32_t>(swapchain_image_index);
        VkPresentInfoKHR present_info;
        GX_SET_ZERO(present_info)
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = frames[frame_number].render_complete.get_vulkan_data_ptr();
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchain.get_vulkan_data_ptr();
        present_info.pImageIndices = &present_image_index;
        const auto present_result = vkQueuePresentKHR(logical_device.get_graphic_queue(), &present_info);
        if (VK_ERROR_OUT_OF_DATE_KHR == present_result) {
            swapchain_image_is_valid = false;
        } else if (VK_SUCCESS != present_result) {
            GX_LOG_F("Presentation failed with result: " << result_to_string(present_result))
        }
    }
}

void gearoenix::vulkan::engine::Engine::upload_imgui_fonts() noexcept
{
    vulkan_upload_imgui_fonts();
}

void gearoenix::vulkan::engine::Engine::create_mesh(
    const std::string& name,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    mesh_manager.create(name, std::move(vertices), std::move(indices), c);
}

gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame() noexcept
{
    return frames[swapchain_image_index];
}

const gearoenix::vulkan::engine::Frame& gearoenix::vulkan::engine::Engine::get_current_frame() const noexcept
{
    return frames[swapchain_image_index];
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

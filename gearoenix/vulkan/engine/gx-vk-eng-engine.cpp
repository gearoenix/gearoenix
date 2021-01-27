#include "gx-vk-eng-engine.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../gx-vk-check.hpp"
#include <imgui_impl_vulkan.h>

gearoenix::vulkan::engine::Engine::Frame::Frame(
    command::Manager& command_manager,
    const Swapchain& swapchain,
    const image::View& depth_stencil,
    const RenderPass& render_pass,
    const unsigned int frame_index) noexcept
    : draw_command(command_manager.create(command::Type::Primary))
    , draw_wait(command_manager.get_logical_device(), true)
    , framebuffer(&swapchain.get_image_views()[frame_index], &depth_stencil, &render_pass)
    , present_complete(command_manager.get_logical_device())
    , render_complete(command_manager.get_logical_device())
{
}

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
    info.GetVulkanProcAddressFn = +[](void*, const char* name) noexcept {
        return Loader::get(name);
    };
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
    GX_VK_CHK_L(vkQueueSubmit(logical_device.get_graphic_queue(), 1, &end_info, nullptr))
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
    , instance(platform_application)
    , surface(instance, platform_application)
    , physical_device(surface)
    , logical_device(physical_device)
    , swapchain(logical_device)
    , memory_manager(logical_device)
    , command_manager(logical_device)
    , descriptor_manager(logical_device)
    , pipeline_manager(logical_device)
    , depth_stencil(image::View::create_depth_stencil(memory_manager))
    , render_pass(swapchain)
{
    initialize_frame();
    //    sampler_manager = std::make_shared<sampler::Manager>(logical_device);
    // main_render_target = vulkan_main_render_target = std::make_shared<texture::MainTarget>(memory_manager, this);
    //    frames_count = static_cast<decltype(frames_count)>(vulkan_main_render_target->get_frames().size());
    // Buffer manager needs the number of frames
    //    image_manager = std::make_shared<image::Manager>(this);
    // buffer_manager = vulkan_buffer_manager = std::make_shared<buffer::Manager>(memory_manager, this);
    //    upload_command_buffers.reserve(static_cast<std::size_t>(frames_count));
    //    upload_semaphore.reserve(static_cast<std::size_t>(frames_count));
    //    for (auto fi = decltype(frames_count) { 0 }; fi < frames_count; ++fi) {
    //        upload_command_buffers.emplace_back(
    //            dynamic_cast<command::Buffer*>(command_manager->create_primary_command_buffer()));
    //        upload_semaphore.push_back(std::make_shared<sync::Semaphore>(logical_device));
    //    }
    setup_imgui();
}

gearoenix::vulkan::engine::Engine::~Engine() noexcept
{
    logical_device.wait_to_finish();
    ImGui_ImplVulkan_Shutdown();
    ImGui::DestroyContext();
    frames.clear();
}

//void gearoenix::render::Engine::window_changed()
//{
//    logical_device->wait_to_finish();
//    swapchain->initialize();
//    depth_stencil = image::View::create_depth_stencil(mem_pool);
//    render_pass = std::shared_ptr<RenderPass>(new RenderPass(swapchain));
//    auto frame_views = swapchain->get_image_views();
//    framebuffers.resize(frames_count);
//    for (uint32_t i = 0; i < frames_count; ++i) {
//        framebuffers[i] = std::shared_ptr<Framebuffer>(
//            new Framebuffer(frame_views[i], depth_stencil, render_pass));
//    }
//    pipeline = std::shared_ptr<pipeline::Pipeline>(new pipeline::Pipeline(
//        pipeline_cache, pipeline_layout, render_pass, shader_manager));
//    setup_draw_buffers();
//}
//
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
        auto& frame = frames[swapchain_image_index];
        if (swapchain_image_is_valid) {
            frame.draw_wait.wait();
            frame.draw_wait.reset();
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
        // Record ImGui commands
        frame.draw_command.begin();
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
        GX_VK_CHK_L(
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
        GX_VK_CHK_L(vkQueuePresentKHR(logical_device.get_graphic_queue(), &present_info))
    }
}

void gearoenix::vulkan::engine::Engine::upload_imgui_fonts() noexcept
{
    vulkan_upload_imgui_fonts();
}

bool gearoenix::vulkan::engine::Engine::is_supported() noexcept
{
    Loader::load();
    return true;
}

#endif

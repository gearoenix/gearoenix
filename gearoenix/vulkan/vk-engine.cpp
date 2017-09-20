#include "vk-engine.hpp"
#include "../core/cr-application.hpp"
#include "../core/cr-static.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-file.hpp"
#include "../system/sys-log.hpp"
#include "buffer/vk-buf-buffer.hpp"
#include "buffer/vk-buf-uniform.hpp"
#include "command/vk-cmd-buffer.hpp"
#include "command/vk-cmd-pool.hpp"
#include "descriptor/vk-des-pool.hpp"
#include "descriptor/vk-des-set.hpp"
#include "device/vk-dev-logical.hpp"
#include "device/vk-dev-physical.hpp"
#include "image/vk-img-image.hpp"
#include "image/vk-img-view.hpp"
#include "memory/vk-mem-manager.hpp"
#include "pipeline/vk-pip-cache.hpp"
#include "pipeline/vk-pip-layout.hpp"
#include "pipeline/vk-pip-pipeline.hpp"
#include "shader/vk-shader-manager.hpp"
#include "sync/vk-sync-fence.hpp"
#include "sync/vk-sync-semaphore.hpp"
#include "vk-check.hpp"
#include "vk-framebuffer.hpp"
#include "vk-instance.hpp"
#include "vk-linker.hpp"
#include "vk-render-pass.hpp"
#include "vk-surface.hpp"
#include "vk-swapchain.hpp"

gearoenix::render::Engine::Engine(system::Application* sys_app)
    : sys_app(sys_app)
{
    linker = new Linker;
    instance = new Instance(linker);
    surface = new Surface(instance, this->sys_app);
    physical_device = new device::Physical(surface);
    logical_device = new device::Logical(physical_device);
    swapchain = new Swapchain(logical_device);
    depth_stencil = image::View::create_depth_stencil(logical_device);
    render_pass = new RenderPass(swapchain);
    const std::vector<image::View*>& frame_views = swapchain->get_image_views();
    framebuffers.resize(frame_views.size());
    for (unsigned int i = 0; i < frame_views.size(); ++i) {
        framebuffers[i] = new Framebuffer(frame_views[i], depth_stencil, render_pass);
    }
    graphic_cmd_pool = new command::Pool(logical_device);
    present_complete_semaphore = new sync::Semaphore(logical_device);
    render_complete_semaphore = new sync::Semaphore(logical_device);
    wait_fences.resize(frame_views.size());
    for (uint32_t i = 0; i < frame_views.size(); ++i) {
        wait_fences[i] = new sync::Fence(logical_device, true);
    }
    //    pipeline_layout = std::shared_ptr<pipeline::Layout>(new pipeline::Layout(logical_device));
    //    pipeline_cache = std::shared_ptr<pipeline::Cache>(new pipeline::Cache(logical_device));
    //    pipeline = std::shared_ptr<pipeline::Pipeline>(new pipeline::Pipeline(
    //        pipeline_cache, pipeline_layout, render_pass, shader_manager));
    //    descriptor_pool = std::shared_ptr<descriptor::Pool>(new descriptor::Pool(logical_device));
    //    descriptor_set = std::shared_ptr<descriptor::Set>(
    //        new descriptor::Set(descriptor_pool, pipeline_layout, uniform));
    //    setup_draw_buffers();
}

gearoenix::render::Engine::~Engine() {}

void gearoenix::render::Engine::window_changed()
{
    //    logical_device->wait_to_finish();
    //    swapchain->initialize();
    //    depth_stencil = image::View::create_depth_stencil(mem_pool);
    //    render_pass = std::shared_ptr<RenderPass>(new RenderPass(swapchain));
    //    auto frame_views = swapchain->get_image_views();
    //    framebuffers.resize(frame_views.size());
    //    for (uint32_t i = 0; i < frame_views.size(); ++i) {
    //        framebuffers[i] = std::shared_ptr<Framebuffer>(
    //            new Framebuffer(frame_views[i], depth_stencil, render_pass));
    //    }
    //    pipeline = std::shared_ptr<pipeline::Pipeline>(new pipeline::Pipeline(
    //        pipeline_cache, pipeline_layout, render_pass, shader_manager));
    //    setup_draw_buffers();
}

void gearoenix::render::Engine::update()
{
    //    uint32_t current_buffer = swapchain->get_next_image_index(present_complete_semaphore);
    //    if (current_buffer == 0xffffffff) {
    //        window_changed();
    //        return;
    //    }
    //    VKC(linker->vkWaitForFences(logical_device->get_vulkan_data(), 1,
    //        &(wait_fences[current_buffer]->get_vulkan_data()),
    //        1, UINT64_MAX));
    //    VKC(linker->vkResetFences(logical_device->get_vulkan_data(), 1,
    //        &(wait_fences[current_buffer]->get_vulkan_data())));
    //    uint32_t wait_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //    VkSubmitInfo submit_info;
    //    setz(submit_info);
    //    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //    submit_info.pWaitDstStageMask = &wait_stage_mask;
    //    submit_info.pWaitSemaphores = &(present_complete_semaphore->get_vulkan_data());
    //    submit_info.waitSemaphoreCount = 1;
    //    submit_info.pSignalSemaphores = &(render_complete_semaphore->get_vulkan_data());
    //    submit_info.signalSemaphoreCount = 1;
    //    submit_info.pCommandBuffers = &(draw_buffers[current_buffer]->get_vulkan_data());
    //    submit_info.commandBufferCount = 1;
    //    VKC(linker->vkQueueSubmit(logical_device->get_graphic_queue(), 1,
    //        &submit_info,
    //        wait_fences[current_buffer]->get_vulkan_data()));
    //    VkPresentInfoKHR present_info;
    //    setz(present_info);
    //    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    //    present_info.swapchainCount = 1;
    //    present_info.pSwapchains = &(swapchain->get_vulkan_data());
    //    present_info.pImageIndices = &current_buffer;
    //    present_info.pWaitSemaphores = &(render_complete_semaphore->get_vulkan_data());
    //    present_info.waitSemaphoreCount = 1;
    //    VKC(linker->vkQueuePresentKHR(logical_device->get_graphic_queue(),
    //        &present_info));
}

void gearoenix::render::Engine::terminate()
{
    logical_device->wait_to_finish();

    for (sync::Fence* f : wait_fences)
        delete f;
    wait_fences.clear();
    delete render_complete_semaphore;
    render_complete_semaphore = nullptr;
    delete present_complete_semaphore;
    present_complete_semaphore = nullptr;
    delete graphic_cmd_pool;
    graphic_cmd_pool = nullptr;
    for (Framebuffer* f : framebuffers)
        delete f;
    framebuffers.clear();
    delete render_pass;
    render_pass = nullptr;
    delete depth_stencil;
    depth_stencil = nullptr;
    delete swapchain;
    swapchain = nullptr;
    delete logical_device;
    logical_device = nullptr;
    delete physical_device;
    physical_device = nullptr;
    delete surface;
    surface = nullptr;
    delete instance;
    instance = nullptr;
    delete linker;
    linker = nullptr;
}

void gearoenix::render::Engine::setup_draw_buffers()
{
    //    VkClearValue clear_values[2];
    //    clear_values[0].color = { { 0.4f, 0.4f, 0.4f, 1.0f } };
    //    clear_values[1].color = { { 1.0f, 0.0f, 0.0f, 0.0f } };
    //    auto surface_caps = physical_device->get_surface_capabilities();
    //    VkRenderPassBeginInfo render_pass_begin_info;
    //    setz(render_pass_begin_info);
    //    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    //    render_pass_begin_info.renderPass = render_pass->get_vulkan_data();
    //    render_pass_begin_info.renderArea.offset.x = 0;
    //    render_pass_begin_info.renderArea.offset.y = 0;
    //    render_pass_begin_info.renderArea.extent.width = surface_caps->currentExtent.width;
    //    render_pass_begin_info.renderArea.extent.height = surface_caps->currentExtent.height;
    //    render_pass_begin_info.clearValueCount = countof(clear_values);
    //    render_pass_begin_info.pClearValues = clear_values;
    //    int images_count = static_cast<int>(framebuffers.size());
    //    draw_buffers.resize(images_count);
    //    for (int i = 0; i < images_count; ++i) {
    //        render_pass_begin_info.framebuffer = framebuffers[i]->get_vulkan_data();
    //        std::shared_ptr<command::Buffer> draw_command(
    //            new command::Buffer(graphic_cmd_pool));
    //        draw_command->begin_render_pass_with_info(render_pass_begin_info);
    //        VkViewport viewport;
    //        viewport.x = 0.0;
    //        viewport.y = 0.0;
    //        viewport.height = static_cast<float>(surface_caps->currentExtent.height);
    //        viewport.width = static_cast<float>(surface_caps->currentExtent.width);
    //        viewport.minDepth = 0.0;
    //        viewport.maxDepth = 1.0;
    //        draw_command->set_viewport(viewport);
    //        VkRect2D scissor;
    //        scissor.extent.width = surface_caps->currentExtent.width;
    //        scissor.extent.height = surface_caps->currentExtent.height;
    //        scissor.offset.x = 0;
    //        scissor.offset.y = 0;
    //        draw_command->set_scissor(scissor);
    //        linker->vkCmdBindDescriptorSets(
    //            draw_command->get_vulkan_data(), VK_PIPELINE_BIND_POINT_GRAPHICS,
    //            pipeline_layout->get_vulkan_data(), 0, 1,
    //            &(descriptor_set->get_vulkan_data()), 0, nullptr);
    //        linker->vkCmdBindPipeline(draw_command->get_vulkan_data(),
    //            VK_PIPELINE_BIND_POINT_GRAPHICS,
    //            pipeline->get_vulkan_data());
    //        VkDeviceSize offsets = 0;
    //        linker->vkCmdBindVertexBuffers(draw_command->get_vulkan_data(), 0, 1,
    //            &(mesh_buff->get_vertex_buffer()), &offsets);
    //        linker->vkCmdBindIndexBuffer(draw_command->get_vulkan_data(),
    //            mesh_buff->get_index_buffer(), 0,
    //            VK_INDEX_TYPE_UINT32);
    //        linker->vkCmdDrawIndexed(draw_command->get_vulkan_data(),
    //            mesh_buff->get_indices_count(), 1, 0, 0, 1);
    //        linker->vkCmdEndRenderPass(draw_command->get_vulkan_data());
    //        VKC(linker->vkEndCommandBuffer(draw_command->get_vulkan_data()));
    //        draw_buffers[i] = draw_command;
    //    }
}

gearoenix::render::device::Logical* gearoenix::render::Engine::get_logical_device()
{
    return logical_device;
}
const gearoenix::render::device::Logical* gearoenix::render::Engine::get_logical_device() const
{
    return logical_device;
}

#include "vk-engine.hpp"
#include "vk-linker.hpp"
#include "vk-instance.hpp"
#include "vk-surface.hpp"
#include "buffer/vk-buf-buffer.hpp"
#include "buffer/vk-buf-uniform.hpp"
#include "command/vk-cmd-pool.hpp"
#include "command/vk-cmd-buffer.hpp"
#include "device/vk-dev-physical.hpp"
#include "device/vk-dev-logical.hpp"
#include "descriptor/vk-des-pool.hpp"
#include "descriptor/vk-des-set.hpp"
#include "image/vk-img-image.hpp"
#include "image/vk-img-view.hpp"
#include "memory/vk-mem-pool.hpp"
#include "pipeline/vk-pip-cache.hpp"
#include "pipeline/vk-pip-layout.hpp"
#include "pipeline/vk-pip-pipeline.hpp"
#include "pipeline/vk-pip-pipeline.hpp"
#include "shader/vk-shader-manager.hpp"
#include "sync/vk-sync-fence.hpp"
#include "sync/vk-sync-semaphore.hpp"
#include "vk-swapchain.hpp"
#include "vk-check.hpp"
#include "vk-framebuffer.hpp"
#include "vk-render-pass.hpp"
#include "../core/cr-application.hpp"
#include "../core/cr-static.hpp"
#include "../system/sys-log.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-file.hpp"

gearoenix::render::Engine::Engine(system::Application *sys_app) : sys_app(sys_app) {
    linker = std::shared_ptr<Linker>(new Linker);
    instance = std::shared_ptr<Instance>(new Instance(linker));
    surface = std::shared_ptr<Surface>(new Surface(instance, this->sys_app));
    physical_device = std::shared_ptr<device::Physical>(new device::Physical(surface));
    logical_device = std::shared_ptr<device::Logical>(new device::Logical(physical_device));
    swapchain = std::shared_ptr<Swapchain>(new Swapchain(logical_device));
    mem_pool = std::shared_ptr<memory::Pool>(new memory::Pool(logical_device));
    depth_stencil = image::View::create_depth_stencil(mem_pool);
    render_pass = std::shared_ptr<RenderPass>(new RenderPass(swapchain));
    auto frame_views = swapchain->get_image_views();
    framebuffers.resize(frame_views.size());
    for (uint32_t i = 0; i < frame_views.size(); ++i) {
        framebuffers[i] = std::shared_ptr<Framebuffer>(
                    new Framebuffer(frame_views[i], depth_stencil, render_pass));
    }
//    LOGE(std::string("reached"));
    graphic_cmd_pool = std::shared_ptr<command::Pool>(new command::Pool(logical_device));
    shader_manager = std::shared_ptr<shader::Manager>(new shader::Manager(sys_app->get_asset()));
//    LOGE(std::string("reached"));
    const float vertices[] = {
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    const uint32_t indices[] = {
        0, 1, 2,
    };
    mesh_buff = std::shared_ptr<buffer::Buffer>(new buffer::Buffer(graphic_cmd_pool, vertices, sizeof(vertices), indices, countof(indices)));
    const float uniform_data[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,

        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,

        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    uniform = std::shared_ptr<buffer::Uniform>(new buffer::Uniform(mem_pool, sizeof(uniform_data)));
    uniform->update(uniform_data);
    pipeline_layout = std::shared_ptr<pipeline::Layout>(new pipeline::Layout(logical_device));
    pipeline_cache = std::shared_ptr<pipeline::Cache>(new pipeline::Cache(logical_device));
    pipeline = std::shared_ptr<pipeline::Pipeline>(new pipeline::Pipeline(pipeline_cache, pipeline_layout, render_pass, shader_manager));
    descriptor_pool = std::shared_ptr<descriptor::Pool>(new descriptor::Pool(logical_device));
    descriptor_set = std::shared_ptr<descriptor::Set>(new descriptor::Set(descriptor_pool, pipeline_layout, uniform));
    present_complete_semaphore = std::shared_ptr<sync::Semaphore>(new sync::Semaphore(logical_device));
    render_complete_semaphore = std::shared_ptr<sync::Semaphore>(new sync::Semaphore(logical_device));
    wait_fences.resize(frame_views.size());
    for (uint32_t i = 0; i < frame_views.size(); ++i) {
        wait_fences[i] = std::shared_ptr<sync::Fence>(new sync::Fence(logical_device, true));
    }
    setup_draw_buffers();
}

gearoenix::render::Engine::~Engine() {}

void gearoenix::render::Engine::window_changed() {
    logical_device->wait_to_finish();
    swapchain->initialize();
    depth_stencil = image::View::create_depth_stencil(mem_pool);
    render_pass = std::shared_ptr<RenderPass>(new RenderPass(swapchain));
    auto frame_views = swapchain->get_image_views();
    framebuffers.resize(frame_views.size());
    for (uint32_t i = 0; i < frame_views.size(); ++i) {
        framebuffers[i] = std::shared_ptr<Framebuffer>(
                    new Framebuffer(frame_views[i], depth_stencil, render_pass));
    }
    pipeline = std::shared_ptr<pipeline::Pipeline>(new pipeline::Pipeline(pipeline_cache, pipeline_layout, render_pass, shader_manager));
    setup_draw_buffers();
}

void gearoenix::render::Engine::update() {
    uint32_t current_buffer = swapchain->get_next_image_index(present_complete_semaphore);
    if(current_buffer == 0xffffffff) {
        window_changed();
        return;
    }
    VKC(linker->vkWaitForFences(logical_device->get_vulkan_data(), 1, &(wait_fences[current_buffer]->get_vulkan_data()), 1, UINT64_MAX));
    VKC(linker->vkResetFences(logical_device->get_vulkan_data(), 1, &(wait_fences[current_buffer]->get_vulkan_data())));
    uint32_t wait_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info;
    setz(submit_info);
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pWaitDstStageMask = &wait_stage_mask;
    submit_info.pWaitSemaphores = &(present_complete_semaphore->get_vulkan_data());
    submit_info.waitSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &(render_complete_semaphore->get_vulkan_data());
    submit_info.signalSemaphoreCount = 1;
    submit_info.pCommandBuffers = &(draw_buffers[current_buffer]->get_vulkan_data());
    submit_info.commandBufferCount = 1;
    VKC(linker->vkQueueSubmit(logical_device->get_graphic_queue(), 1, &submit_info, wait_fences[current_buffer]->get_vulkan_data()));
    VkPresentInfoKHR present_info;
    setz(present_info);
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &(swapchain->get_vulkan_data());
    present_info.pImageIndices = &current_buffer;
    present_info.pWaitSemaphores = &(render_complete_semaphore->get_vulkan_data());
    present_info.waitSemaphoreCount = 1;
    VKC(linker->vkQueuePresentKHR(logical_device->get_graphic_queue(), &present_info));
}

void gearoenix::render::Engine::terminate() {
    logical_device->wait_to_finish();
    wait_fences.clear();
    render_complete_semaphore = nullptr;
    present_complete_semaphore = nullptr;
    descriptor_set = nullptr;
    descriptor_pool = nullptr;
    pipeline = nullptr;
    pipeline_cache = nullptr;
    pipeline_layout = nullptr;
    uniform = nullptr;
    mesh_buff = nullptr;
    shader_manager = nullptr;
    graphic_cmd_pool = nullptr;
    framebuffers.clear();
    render_pass = nullptr;
    depth_stencil = nullptr;
    mem_pool = nullptr;
    swapchain = nullptr;
    logical_device = nullptr;
    physical_device = nullptr;
    surface = nullptr;
    instance = nullptr;
    linker = nullptr;
}

void gearoenix::render::Engine::setup_draw_buffers() {
    VkClearValue clear_values[2];
    clear_values[0].color = {{0.4f, 0.4f, 0.4f, 1.0f}};
    clear_values[1].color = {{1.0f, 0.0f, 0.0f, 0.0f}};
    auto surface_caps = physical_device->get_surface_capabilities();
    VkRenderPassBeginInfo render_pass_begin_info;
    setz(render_pass_begin_info);
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = render_pass->get_vulkan_data();
    render_pass_begin_info.renderArea.offset.x = 0;
    render_pass_begin_info.renderArea.offset.y = 0;
    render_pass_begin_info.renderArea.extent.width = surface_caps->currentExtent.width;
    render_pass_begin_info.renderArea.extent.height = surface_caps->currentExtent.height;
    render_pass_begin_info.clearValueCount = countof(clear_values);
    render_pass_begin_info.pClearValues = clear_values;
    int images_count = static_cast<int>(framebuffers.size());
    draw_buffers.resize(images_count);
    for(int i = 0; i < images_count; ++i) {
        render_pass_begin_info.framebuffer = framebuffers[i]->get_vulkan_data();
        std::shared_ptr<command::Buffer> draw_command(new command::Buffer(graphic_cmd_pool));
        draw_command->begin_render_pass_with_info(render_pass_begin_info);
        VkViewport viewport;
        viewport.x = 0.0;
        viewport.y = 0.0;
        viewport.height = static_cast<float>(surface_caps->currentExtent.height);
        viewport.width = static_cast<float>(surface_caps->currentExtent.width);
        viewport.minDepth = 0.0;
        viewport.maxDepth = 1.0;
        draw_command->set_viewport(viewport);
        VkRect2D scissor;
        scissor.extent.width = surface_caps->currentExtent.width;
        scissor.extent.height = surface_caps->currentExtent.height;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        draw_command->set_scissor(scissor);
        linker->vkCmdBindDescriptorSets(
                    draw_command->get_vulkan_data(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline_layout->get_vulkan_data(), 0, 1,
                    &(descriptor_set->get_vulkan_data()), 0, nullptr);
        linker->vkCmdBindPipeline(
                    draw_command->get_vulkan_data(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline->get_vulkan_data());
        VkDeviceSize offsets = 0;
        linker->vkCmdBindVertexBuffers(
                    draw_command->get_vulkan_data(), 0, 1, &(mesh_buff->get_vertex_buffer()), &offsets);
        linker->vkCmdBindIndexBuffer(
                    draw_command->get_vulkan_data(), mesh_buff->get_index_buffer(), 0, VK_INDEX_TYPE_UINT32);
        linker->vkCmdDrawIndexed(
                    draw_command->get_vulkan_data(), mesh_buff->get_indices_count(), 1, 0, 0, 1);
        linker->vkCmdEndRenderPass(draw_command->get_vulkan_data());
        VKC(linker->vkEndCommandBuffer(draw_command->get_vulkan_data()));
        draw_buffers[i] = draw_command;
    }
}

#include "gx-vk-eng-engine.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/gx-cr-application.hpp"
#include "../../render/scene/gx-rnd-scn-scene.hpp"
#include "../../system/gx-sys-application.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../command/gx-vk-cmd-pool.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-manager.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../sampler/gx-vk-smp-manager.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "../texture/gx-vk-txt-main-target.hpp"

gearoenix::vulkan::engine::Engine::Engine(system::Application* const sys_app) noexcept
    : render::engine::Engine(sys_app, render::engine::Type::Vulkan)
{
    Loader::load();
    instance = std::make_shared<Instance>(sys_app);
    surface = std::make_shared<Surface>(instance, sys_app);
    physical_device = std::make_shared<device::Physical>(surface);
    logical_device = std::make_shared<device::Logical>(physical_device);
    memory_manager = memory::Manager::construct(logical_device);
    image_manager = std::make_shared<image::Manager>();
    sampler_manager = std::make_shared<sampler::Manager>(logical_device);
    command_manager = std::make_unique<command::Manager>(logical_device);
    main_render_target = std::make_shared<texture::MainTarget>(*memory_manager, this);
    frames_count = dynamic_cast<texture::MainTarget*>(main_render_target.get())->get_frames().size();
    // Buffer manager needs the number of frames
    vulkan_buffer_manager = std::make_unique<buffer::Manager>(memory_manager, this);
    buffer_manager = vulkan_buffer_manager;
}

gearoenix::vulkan::engine::Engine* gearoenix::vulkan::engine::Engine::construct(
    system::Application* sys_app) noexcept
{
    return new Engine(sys_app);
}

gearoenix::vulkan::engine::Engine::~Engine() noexcept = default;

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
void gearoenix::vulkan::engine::Engine::update() noexcept { }
//{
//    LOGE("TODO: it can be much much better remove all duplicate calls, remove same initializations.");
//    const VkDevice vkdev = logical_device->get_vulkan_data();
//    const VkQueue vkqu = logical_device->get_graphic_queue();
//    current_frame = swapchain->get_next_image_index(present_complete_semaphore);
//    if (current_frame == 0xffffffff) {
//        window_changed();
//        return;
//    }
//    const VkFence vkcurfnc = wait_fences[current_frame]->get_vulkan_data();
//
//    VKC(linker->vkWaitForFences(vkdev, 1, &vkcurfnc, 1, UINT64_MAX));
//    VKC(linker->vkResetFences(vkdev, 1, &vkcurfnc));
//
//    command::Buffer* gcmd = cmd_bufs[current_frame];
//    gcmd->begin();
//
//    std::vector<std::function<void()>>& cur_frames_cleanup = frames_cleanups[current_frame];
//    for (std::function<void()>& fn : cur_frames_cleanup) {
//        fn();
//    }
//    cur_frames_cleanup.clear();
//    std::vector<std::function<std::function<void()>(command::Buffer*)>> temp_todos;
//    todos_mutex.lock();
//    std::move(todos.begin(), todos.end(), std::back_inserter(temp_todos));
//    todos.clear();
//    todos_mutex.unlock();
//    for (std::function<std::function<void()>(command::Buffer*)>& fn : temp_todos) {
//        frames_cleanups[current_frame].push_back(fn(gcmd));
//    }
//
//    for (std::shared_ptr<scene::Scene>& s : loaded_scenes) {
//        s->commit();
//    }
//
//    //////////////////////////////////////////////////////////////////////////////////////
//    VkBufferMemoryBarrier buffer_memory_barrier;
//    setz(buffer_memory_barrier);
//    buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
//    buffer_memory_barrier.srcAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
//    buffer_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//    buffer_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    buffer_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    buffer_memory_barrier.buffer = uvbufmgr[current_frame]->get_buffer()->get_vulkan_data();
//    buffer_memory_barrier.size = (VkDeviceSize)uvbufmgr[current_frame]->get_size();
//    buffer_memory_barrier.offset = (VkDeviceSize)uvbufmgr[current_frame]->get_offset();
//    linker->vkCmdPipelineBarrier(
//        gcmd->get_vulkan_data(), // command buffer
//        VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, // srcStageMask
//        VK_PIPELINE_STAGE_TRANSFER_BIT, // dstStageMask
//        0, // dependencyFlags
//        0, // memoryBarrierCount
//        nullptr, // pMemoryBarriers
//        1, // bufferMemoryBarrierCount
//        &buffer_memory_barrier, // pBufferMemoryBarriers
//        0, // imageMemoryBarrierCount
//        nullptr // pImageMemoryBarriers
//    );
//    VkBufferCopy unicpyinf;
//    unicpyinf.dstOffset = uvbufmgr[current_frame]->get_offset();
//    unicpyinf.srcOffset = ucbufmgr[current_frame]->get_offset();
//    unicpyinf.size = uvbufmgr[current_frame]->get_size();
//    gcmd->copy_buffer(cbufmgr->get_buffer()->get_vulkan_data(), vbufmgr->get_buffer()->get_vulkan_data(), unicpyinf);
//    buffer_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//    buffer_memory_barrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
//    linker->vkCmdPipelineBarrier(
//        gcmd->get_vulkan_data(), // command buffer
//        VK_PIPELINE_STAGE_TRANSFER_BIT, // srcStageMask
//        VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, // dstStageMask
//        0, // dependencyFlags
//        0, // memoryBarrierCount
//        nullptr, // pMemoryBarriers
//        1, // bufferMemoryBarrierCount
//        &buffer_memory_barrier, // pBufferMemoryBarriers
//        0, // imageMemoryBarrierCount
//        nullptr // pImageMemoryBarriers
//    );
//    //////////////////////////////////////////////////////////////////////////////////
//
//    VkClearValue clear_values[2];
//    clear_values[0].color = { { 0.4f, 0.4f, 0.4f, 1.0f } };
//    clear_values[1].color = { { 1.0f, 0.0f, 0.0f, 0.0f } };
//    const VkSurfaceCapabilitiesKHR& surface_caps = physical_device->get_surface_capabilities();
//    VkRenderPassBeginInfo render_pass_begin_info;
//    setz(render_pass_begin_info);
//    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//    render_pass_begin_info.renderPass = render_pass->get_vulkan_data();
//    render_pass_begin_info.renderArea.offset.x = 0;
//    render_pass_begin_info.renderArea.offset.y = 0;
//    render_pass_begin_info.renderArea.extent.width = surface_caps.currentExtent.width;
//    render_pass_begin_info.renderArea.extent.height = surface_caps.currentExtent.height;
//    render_pass_begin_info.clearValueCount = countof(clear_values);
//    render_pass_begin_info.pClearValues = clear_values;
//    render_pass_begin_info.framebuffer = framebuffers[current_frame]->get_vulkan_data();
//    gcmd->begin_render_pass_with_info(render_pass_begin_info);
//    VkViewport viewport;
//    viewport.x = 0.0;
//    viewport.y = 0.0;
//    viewport.height = static_cast<float>(surface_caps.currentExtent.height);
//    viewport.width = static_cast<float>(surface_caps.currentExtent.width);
//    viewport.minDepth = 0.0;
//    viewport.maxDepth = 1.0;
//    gcmd->set_viewport(viewport);
//    VkRect2D scissor;
//    scissor.extent.width = surface_caps.currentExtent.width;
//    scissor.extent.height = surface_caps.currentExtent.height;
//    scissor.offset.x = 0;
//    scissor.offset.y = 0;
//    gcmd->set_scissor(scissor);
//    for (std::shared_ptr<scene::Scene>& s : loaded_scenes) {
//        s->draw();
//    }
//    //    linker->vkCmdBindDescriptorSets(
//    //        draw_command->get_vulkan_data(), VK_PIPELINE_BIND_POINT_GRAPHICS,
//    //        pipeline_layout->get_vulkan_data(), 0, 1,
//    //        &(descriptor_set->get_vulkan_data()), 0, nullptr);
//    //    linker->vkCmdBindPipeline(draw_command->get_vulkan_data(),
//    //        VK_PIPELINE_BIND_POINT_GRAPHICS,
//    //        pipeline->get_vulkan_data());
//    //    VkDeviceSize offsets = 0;
//    //    linker->vkCmdBindVertexBuffers(draw_command->get_vulkan_data(), 0, 1,
//    //        &(mesh_buff->get_vertex_buffer()), &offsets);
//    //    linker->vkCmdBindIndexBuffer(draw_command->get_vulkan_data(),
//    //        mesh_buff->get_index_buffer(), 0,
//    //        VK_INDEX_TYPE_UINT32);
//    //    linker->vkCmdDrawIndexed(draw_command->get_vulkan_data(),
//    //        mesh_buff->get_indices_count(), 1, 0, 0, 1);
//
//    gcmd->end_render_pass();
//    gcmd->end();
//
//    uint32_t wait_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    VkSubmitInfo submit_info;
//    setz(submit_info);
//    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    submit_info.pWaitDstStageMask = &wait_stage_mask;
//    submit_info.pWaitSemaphores = &(present_complete_semaphore->get_vulkan_data());
//    submit_info.waitSemaphoreCount = 1;
//    submit_info.pSignalSemaphores = &(render_complete_semaphore->get_vulkan_data());
//    submit_info.signalSemaphoreCount = 1;
//    submit_info.pCommandBuffers = &gcmd->get_vulkan_data();
//    submit_info.commandBufferCount = 1;
//    VKC(linker->vkQueueSubmit(vkqu, 1, &submit_info, vkcurfnc));
//    VkPresentInfoKHR present_info;
//    setz(present_info);
//    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//    present_info.swapchainCount = 1;
//    present_info.pSwapchains = &(swapchain->get_vulkan_data());
//    present_info.pImageIndices = &current_frame;
//    present_info.pWaitSemaphores = &(render_complete_semaphore->get_vulkan_data());
//    present_info.waitSemaphoreCount = 1;
//    VKC(linker->vkQueuePresentKHR(vkqu, &present_info));
//}
//
void gearoenix::vulkan::engine::Engine::terminate() noexcept { }

std::shared_ptr<gearoenix::render::sync::Semaphore> gearoenix::vulkan::engine::Engine::create_semaphore() const noexcept
{
    return std::make_shared<sync::Semaphore>(logical_device);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::vulkan::engine::Engine::create_texture_2d(
    const core::Id id,
    std::string name,
    std::vector<std::vector<std::uint8_t>> data,
    const render::texture::TextureInfo& info,
    const std::size_t img_width,
    const std::size_t img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    return std::make_shared<texture::Texture2D>(id, std::move(name), this, data, info, img_width, img_height, call);
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::vulkan::engine::Engine::create_texture_cube(
    const core::Id id,
    std::string name,
    std::vector<std::vector<std::vector<std::uint8_t>>> data,
    const render::texture::TextureInfo& info,
    std::size_t aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept {
    GX_UNIMPLEMENTED
}

std::shared_ptr<gearoenix::render::texture::Target> gearoenix::vulkan::engine::Engine::create_render_target(
    const core::Id id,
    const std::vector<render::texture::AttachmentInfo>& infos,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::vulkan::engine::Engine::submit(
    const std::size_t pres_count,
    const render::sync::Semaphore* const* pres,
    const std::size_t cmds_count,
    const render::command::Buffer* const* cmds,
    const std::size_t nxts_count,
    const render::sync::Semaphore* const* nxts) noexcept
{
    GX_UNIMPLEMENTED
}

//void gearoenix::render::Engine::setup_draw_buffers()
//{
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
//}

#endif

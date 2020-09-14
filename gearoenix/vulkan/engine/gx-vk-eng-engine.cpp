#include "gx-vk-eng-engine.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-application.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../render/scene/rnd-scn-scene.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/vk-buf-buffer.hpp"
#include "../buffer/vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-pool.hpp"
#include "../descriptor/vk-des-pool.hpp"
#include "../descriptor/vk-des-set.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-framebuffer.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-loader.hpp"
#include "../gx-vk-render-pass.hpp"
#include "../gx-vk-surface.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../image/vk-img-image.hpp"
#include "../image/vk-img-view.hpp"
#include "../memory/vk-mem-manager.hpp"
#include "../pipeline/vk-pip-manager.hpp"
#include "../sync/vk-sync-fence.hpp"
#include "../sync/vk-sync-semaphore.hpp"
#include "../texture/vk-txt-sampler-2d.hpp"

gearoenix::vulkan::engine::Engine::Engine(system::Application* sys_app) noexcept
    : render::engine::Engine(sys_app, render::engine::Type::Vulkan)
{
    linker = new Linker();
    instance = new Instance(linker);
    surface = new Surface(instance, this->sys_app);
    physical_device = new device::Physical(surface);
    logical_device = new device::Logical(physical_device);
    swapchain = new Swapchain(logical_device);
    depth_stencil = image::View::create_depth_stencil(logical_device);
    render_pass = new RenderPass(swapchain);
    const std::vector<image::View*>& frame_views = swapchain->get_image_views();
    graphic_cmd_pool = new command::Pool(logical_device);
    vmemmgr = new memory::Manager(logical_device, 1024 * 1024 * 10);
    cmemmgr = new memory::Manager(logical_device, 1024 * 1024 * 10, memory::Manager::CPU_COHERENT);
    vbufmgr = new buffer::Manager(vmemmgr, 5 * 1024 * 1024);
    cbufmgr = new buffer::Manager(cmemmgr, 10 * 1024 * 1024);
    frames_count = frame_views.size();
    framebuffers.resize(frames_count);
    wait_fences.resize(frames_count);
    frames_cleanups.resize(frames_count);
    cmd_bufs.resize(frames_count);
    ucbufmgr.resize(frames_count);
    uvbufmgr.resize(frames_count);
    for (unsigned int i = 0; i < frames_count; ++i) {
        framebuffers[i] = new Framebuffer(frame_views[i], depth_stencil, render_pass);
        wait_fences[i] = new sync::Fence(logical_device, true);
        cmd_bufs[i] = new command::Buffer(graphic_cmd_pool);
        ucbufmgr[i] = new buffer::Manager(1024 * 1024, cbufmgr);
        uvbufmgr[i] = new buffer::Manager(1024 * 1024, vbufmgr);
    }
    present_complete_semaphore = new sync::Semaphore(logical_device);
    render_complete_semaphore = new sync::Semaphore(logical_device);
    pipmgr = new pipeline::Manager(this);
    sampler_2d = new texture::Sampler2D(logical_device);
    //    setup_draw_buffers();
}

gearoenix::render::Engine::~Engine() { }

void gearoenix::render::Engine::window_changed()
{
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
}

void gearoenix::render::Engine::update()
{
    LOGE("TODO: it can be much much better remove all duplicate calls, remove same initializations.");
    const VkDevice vkdev = logical_device->get_vulkan_data();
    const VkQueue vkqu = logical_device->get_graphic_queue();
    current_frame = swapchain->get_next_image_index(present_complete_semaphore);
    if (current_frame == 0xffffffff) {
        window_changed();
        return;
    }
    const VkFence vkcurfnc = wait_fences[current_frame]->get_vulkan_data();

    VKC(linker->vkWaitForFences(vkdev, 1, &vkcurfnc, 1, UINT64_MAX));
    VKC(linker->vkResetFences(vkdev, 1, &vkcurfnc));

    command::Buffer* gcmd = cmd_bufs[current_frame];
    gcmd->begin();

    std::vector<std::function<void()>>& cur_frames_cleanup = frames_cleanups[current_frame];
    for (std::function<void()>& fn : cur_frames_cleanup) {
        fn();
    }
    cur_frames_cleanup.clear();
    std::vector<std::function<std::function<void()>(command::Buffer*)>> temp_todos;
    todos_mutex.lock();
    std::move(todos.begin(), todos.end(), std::back_inserter(temp_todos));
    todos.clear();
    todos_mutex.unlock();
    for (std::function<std::function<void()>(command::Buffer*)>& fn : temp_todos) {
        frames_cleanups[current_frame].push_back(fn(gcmd));
    }

    for (std::shared_ptr<scene::Scene>& s : loaded_scenes) {
        s->commit();
    }

    //////////////////////////////////////////////////////////////////////////////////////
    VkBufferMemoryBarrier buffer_memory_barrier;
    setz(buffer_memory_barrier);
    buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    buffer_memory_barrier.srcAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
    buffer_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    buffer_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    buffer_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    buffer_memory_barrier.buffer = uvbufmgr[current_frame]->get_buffer()->get_vulkan_data();
    buffer_memory_barrier.size = (VkDeviceSize)uvbufmgr[current_frame]->get_size();
    buffer_memory_barrier.offset = (VkDeviceSize)uvbufmgr[current_frame]->get_offset();
    linker->vkCmdPipelineBarrier(
        gcmd->get_vulkan_data(), // command buffer
        VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, // srcStageMask
        VK_PIPELINE_STAGE_TRANSFER_BIT, // dstStageMask
        0, // dependencyFlags
        0, // memoryBarrierCount
        nullptr, // pMemoryBarriers
        1, // bufferMemoryBarrierCount
        &buffer_memory_barrier, // pBufferMemoryBarriers
        0, // imageMemoryBarrierCount
        nullptr // pImageMemoryBarriers
    );
    VkBufferCopy unicpyinf;
    unicpyinf.dstOffset = uvbufmgr[current_frame]->get_offset();
    unicpyinf.srcOffset = ucbufmgr[current_frame]->get_offset();
    unicpyinf.size = uvbufmgr[current_frame]->get_size();
    gcmd->copy_buffer(cbufmgr->get_buffer()->get_vulkan_data(), vbufmgr->get_buffer()->get_vulkan_data(), unicpyinf);
    buffer_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    buffer_memory_barrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
    linker->vkCmdPipelineBarrier(
        gcmd->get_vulkan_data(), // command buffer
        VK_PIPELINE_STAGE_TRANSFER_BIT, // srcStageMask
        VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, // dstStageMask
        0, // dependencyFlags
        0, // memoryBarrierCount
        nullptr, // pMemoryBarriers
        1, // bufferMemoryBarrierCount
        &buffer_memory_barrier, // pBufferMemoryBarriers
        0, // imageMemoryBarrierCount
        nullptr // pImageMemoryBarriers
    );
    //////////////////////////////////////////////////////////////////////////////////

    VkClearValue clear_values[2];
    clear_values[0].color = { { 0.4f, 0.4f, 0.4f, 1.0f } };
    clear_values[1].color = { { 1.0f, 0.0f, 0.0f, 0.0f } };
    const VkSurfaceCapabilitiesKHR& surface_caps = physical_device->get_surface_capabilities();
    VkRenderPassBeginInfo render_pass_begin_info;
    setz(render_pass_begin_info);
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = render_pass->get_vulkan_data();
    render_pass_begin_info.renderArea.offset.x = 0;
    render_pass_begin_info.renderArea.offset.y = 0;
    render_pass_begin_info.renderArea.extent.width = surface_caps.currentExtent.width;
    render_pass_begin_info.renderArea.extent.height = surface_caps.currentExtent.height;
    render_pass_begin_info.clearValueCount = countof(clear_values);
    render_pass_begin_info.pClearValues = clear_values;
    render_pass_begin_info.framebuffer = framebuffers[current_frame]->get_vulkan_data();
    gcmd->begin_render_pass_with_info(render_pass_begin_info);
    VkViewport viewport;
    viewport.x = 0.0;
    viewport.y = 0.0;
    viewport.height = static_cast<float>(surface_caps.currentExtent.height);
    viewport.width = static_cast<float>(surface_caps.currentExtent.width);
    viewport.minDepth = 0.0;
    viewport.maxDepth = 1.0;
    gcmd->set_viewport(viewport);
    VkRect2D scissor;
    scissor.extent.width = surface_caps.currentExtent.width;
    scissor.extent.height = surface_caps.currentExtent.height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    gcmd->set_scissor(scissor);
    for (std::shared_ptr<scene::Scene>& s : loaded_scenes) {
        s->draw();
    }
    //    linker->vkCmdBindDescriptorSets(
    //        draw_command->get_vulkan_data(), VK_PIPELINE_BIND_POINT_GRAPHICS,
    //        pipeline_layout->get_vulkan_data(), 0, 1,
    //        &(descriptor_set->get_vulkan_data()), 0, nullptr);
    //    linker->vkCmdBindPipeline(draw_command->get_vulkan_data(),
    //        VK_PIPELINE_BIND_POINT_GRAPHICS,
    //        pipeline->get_vulkan_data());
    //    VkDeviceSize offsets = 0;
    //    linker->vkCmdBindVertexBuffers(draw_command->get_vulkan_data(), 0, 1,
    //        &(mesh_buff->get_vertex_buffer()), &offsets);
    //    linker->vkCmdBindIndexBuffer(draw_command->get_vulkan_data(),
    //        mesh_buff->get_index_buffer(), 0,
    //        VK_INDEX_TYPE_UINT32);
    //    linker->vkCmdDrawIndexed(draw_command->get_vulkan_data(),
    //        mesh_buff->get_indices_count(), 1, 0, 0, 1);

    gcmd->end_render_pass();
    gcmd->end();

    uint32_t wait_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info;
    setz(submit_info);
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pWaitDstStageMask = &wait_stage_mask;
    submit_info.pWaitSemaphores = &(present_complete_semaphore->get_vulkan_data());
    submit_info.waitSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &(render_complete_semaphore->get_vulkan_data());
    submit_info.signalSemaphoreCount = 1;
    submit_info.pCommandBuffers = &gcmd->get_vulkan_data();
    submit_info.commandBufferCount = 1;
    VKC(linker->vkQueueSubmit(vkqu, 1, &submit_info, vkcurfnc));
    VkPresentInfoKHR present_info;
    setz(present_info);
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &(swapchain->get_vulkan_data());
    present_info.pImageIndices = &current_frame;
    present_info.pWaitSemaphores = &(render_complete_semaphore->get_vulkan_data());
    present_info.waitSemaphoreCount = 1;
    VKC(linker->vkQueuePresentKHR(vkqu, &present_info));
}

void gearoenix::render::Engine::terminate()
{
    // GXTODO think about todos
    // GXTODO think about cleanups
    logical_device->wait_to_finish();
    for (buffer::Manager* u : uvbufmgr)
        delete u;
    for (buffer::Manager* u : ucbufmgr)
        delete u;
    for (command::Buffer* c : cmd_bufs)
        delete c;
    delete sampler_2d;
    sampler_2d = nullptr;
    delete pipmgr;
    pipmgr = nullptr;
    delete cbufmgr;
    cbufmgr = nullptr;
    delete vbufmgr;
    vbufmgr = nullptr;
    delete cmemmgr;
    cmemmgr = nullptr;
    delete vmemmgr;
    vmemmgr = nullptr;
    for (sync::Fence* format : wait_fences)
        delete format;
    wait_fences.clear();
    delete render_complete_semaphore;
    render_complete_semaphore = nullptr;
    delete present_complete_semaphore;
    present_complete_semaphore = nullptr;
    delete graphic_cmd_pool;
    graphic_cmd_pool = nullptr;
    for (Framebuffer* format : framebuffers)
        delete format;
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

const gearoenix::render::Linker* gearoenix::render::Engine::get_linker() const
{
    return linker;
}

gearoenix::render::device::Logical* gearoenix::render::Engine::get_logical_device()
{
    return logical_device;
}

const gearoenix::render::device::Logical* gearoenix::render::Engine::get_logical_device() const
{
    return logical_device;
}

gearoenix::render::RenderPass* gearoenix::render::Engine::get_render_pass()
{
    return render_pass;
}

gearoenix::render::buffer::Manager* gearoenix::render::Engine::get_gpu_buffer_manager()
{
    return vbufmgr;
}

gearoenix::render::buffer::Manager* gearoenix::render::Engine::get_cpu_buffer_manager()
{
    return cbufmgr;
}

gearoenix::render::buffer::Manager* gearoenix::render::Engine::get_uniform_gpu_buffer_manager(unsigned int i)
{
    return uvbufmgr[i];
}

gearoenix::render::buffer::Manager* gearoenix::render::Engine::get_uniform_cpu_buffer_manager(unsigned int i)
{
    return ucbufmgr[i];
}

gearoenix::system::Application* gearoenix::render::Engine::get_system_application()
{
    return sys_app;
}

gearoenix::render::memory::Manager* gearoenix::render::Engine::get_v_memory_manager()
{
    return vmemmgr;
}

gearoenix::render::memory::Manager* gearoenix::render::Engine::get_cpu_memory_manager()
{
    return cmemmgr;
}

gearoenix::render::pipeline::Manager* gearoenix::render::Engine::get_pipeline_manager()
{
    return pipmgr;
}

const gearoenix::render::pipeline::Manager* gearoenix::render::Engine::get_pipeline_manager() const
{
    return pipmgr;
}

const gearoenix::render::texture::Sampler2D* gearoenix::render::Engine::get_sampler_2d() const
{
    return sampler_2d;
}

gearoenix::render::texture::Sampler2D* gearoenix::render::Engine::get_sampler_2d()
{
    return sampler_2d;
}

unsigned int gearoenix::render::Engine::get_frames_count() const
{
    return framebuffers.size();
}

unsigned int gearoenix::render::Engine::load_scene(core::Id scene_id, std::function<void(unsigned int)> on_load)
{
    unsigned int result = loaded_scenes.size();
    loaded_scenes.push_back(sys_app->get_asset_manager()->get_scene(scene_id, core::sync::EndCaller::create([this, result, on_load] {
        loaded_scenes[result]->set_renderable(true);
        on_load(result);
    })));
    return result;
}

void gearoenix::render::Engine::push_todo(std::function<std::function<void()>(command::Buffer*)> fun)
{
    std::lock_guard<std::mutex> lock(todos_mutex);
    todos.push_back(fun);
    (void)lock;
}

unsigned int gearoenix::render::Engine::get_current_frame_index() const
{
    return current_frame;
}

gearoenix::render::command::Buffer* gearoenix::render::Engine::get_current_command_buffer()
{
    return cmd_bufs[current_frame];
}
#endif

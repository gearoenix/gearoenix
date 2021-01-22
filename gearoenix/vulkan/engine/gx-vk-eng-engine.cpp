#include "gx-vk-eng-engine.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
//#include "../../core/asset/gx-cr-asset-manager.hpp"
//#include "../../core/gx-cr-application.hpp"
//#include "../../render/scene/gx-rnd-scn-scene.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/gx-plt-log.hpp"
//#include "../buffer/gx-vk-buf-buffer.hpp"
//#include "../buffer/gx-vk-buf-manager.hpp"
//#include "../command/gx-vk-cmd-buffer.hpp"
//#include "../command/gx-vk-cmd-manager.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
//#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-surface.hpp"
//#include "../gx-vk-loader.hpp"
//#include "../image/gx-vk-img-image.hpp"
//#include "../image/gx-vk-img-manager.hpp"
//#include "../memory/gx-vk-mem-manager.hpp"
//#include "../sampler/gx-vk-smp-manager.hpp"
//#include "../sync/gx-vk-sync-semaphore.hpp"
//#include "../texture/gx-vk-txt-2d.hpp"
//#include "../texture/gx-vk-txt-cube.hpp"
//#include "../texture/gx-vk-txt-main-target.hpp"
//#include "../texture/gx-vk-txt-target.hpp"

gearoenix::vulkan::engine::Engine::Engine(
    const render::RuntimeConfiguration& configuration,
    std::shared_ptr<platform::Application> plt_app) noexcept
    : render::engine::Engine(configuration, std::move(plt_app), render::engine::Type::Vulkan)
    , instance(std::make_shared<Instance>(platform_application))
    , surface(std::make_shared<Surface>(instance, platform_application))
    , physical_device(std::make_shared<device::Physical>(surface))
    , logical_device(std::make_shared<device::Logical>(physical_device))
//    , memory_manager(logical_device)
{
    ;
    //    sampler_manager = std::make_shared<sampler::Manager>(logical_device);
    // command_manager = std::make_unique<command::Manager>(logical_device);
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
}

std::shared_ptr<gearoenix::vulkan::engine::Engine> gearoenix::vulkan::engine::Engine::construct(
    const render::RuntimeConfiguration& configuration,
    std::shared_ptr<platform::Application> platform_application) noexcept
{
    return std::shared_ptr<Engine>(new Engine(configuration, std::move(platform_application)));
}

gearoenix::vulkan::engine::Engine::~Engine() noexcept
{
    logical_device->wait_to_finish();
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
void gearoenix::vulkan::engine::Engine::update() noexcept
{
    render::engine::Engine::update();
    //    vulkan_main_render_target->update();
    //    auto* const up_cmd = upload_command_buffers[frame_number].get();
    //    up_cmd->begin();
    //    image_manager->update(*up_cmd);
    //    up_cmd->end();
    //    const render::sync::Semaphore* const pre_sem = vulkan_main_render_target->get_current_frame()->start_semaphore.get();
    //    const render::sync::Semaphore* const nxt_sem = upload_semaphore[frame_number].get();
    //    const render::command::Buffer* const cmd = up_cmd;
    //    submit(1, &pre_sem, 1, &cmd, 1, &nxt_sem);
}

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
// submission
// present of main target

//std::shared_ptr<gearoenix::render::sync::Semaphore> gearoenix::vulkan::engine::Engine::create_semaphore() const noexcept
//{
//    return std::make_shared<sync::Semaphore>(logical_device);
//}
//
//std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::vulkan::engine::Engine::create_texture_2d(
//    const core::Id id,
//    std::string name,
//    std::vector<std::vector<std::uint8_t>> data,
//    const render::texture::TextureInfo& info,
//    const std::size_t img_width,
//    const std::size_t img_height,
//    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
//{
//    return std::make_shared<texture::Texture2D>(id, std::move(name), this, data, info, img_width, img_height, call);
//}
//
//std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::vulkan::engine::Engine::create_texture_cube(
//    const core::Id id,
//    std::string name,
//    std::vector<std::vector<std::vector<std::uint8_t>>> data,
//    const render::texture::TextureInfo& info,
//    std::size_t aspect,
//    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
//{
//    return std::make_shared<texture::TextureCube>(id, std::move(name), this, data, info, aspect, call);
//}
//
//std::shared_ptr<gearoenix::render::texture::Target> gearoenix::vulkan::engine::Engine::create_render_target(
//    const core::Id id,
//    const std::vector<render::texture::AttachmentInfo>& infos,
//    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
//{
//    return std::make_shared<texture::Target>(id, infos, this, call);
//}
//
//void gearoenix::vulkan::engine::Engine::submit(
//    const std::size_t pres_count,
//    const render::sync::Semaphore* const* const pres,
//    const std::size_t cmds_count,
//    const render::command::Buffer* const* const cmds,
//    const std::size_t nxts_count,
//    const render::sync::Semaphore* const* const nxts) noexcept
//{
//    std::vector<VkSemaphore> previous_semaphores(pres_count), next_semaphores(nxts_count);
//    std::vector<VkCommandBuffer> commands(cmds_count);
//    for (auto i = decltype(pres_count) { 0 }; i < pres_count; ++i)
//        previous_semaphores[i] = dynamic_cast<const sync::Semaphore*>(pres[i])->get_vulkan_data();
//    for (auto i = decltype(nxts_count) { 0 }; i < nxts_count; ++i)
//        next_semaphores[i] = dynamic_cast<const sync::Semaphore*>(nxts[i])->get_vulkan_data();
//    for (auto i = decltype(cmds_count) { 0 }; i < cmds_count; ++i)
//        commands[i] = dynamic_cast<const command::Buffer*>(cmds[i])->get_vulkan_data();
//    std::uint32_t wait_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    VkSubmitInfo info;
//    GX_SET_ZERO(info)
//    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    info.pWaitDstStageMask = &wait_stage_mask;
//    info.pWaitSemaphores = previous_semaphores.data();
//    info.waitSemaphoreCount = static_cast<std::uint32_t>(previous_semaphores.size());
//    info.pSignalSemaphores = next_semaphores.data();
//    info.signalSemaphoreCount = static_cast<std::uint32_t>(next_semaphores.size());
//    info.pCommandBuffers = commands.data();
//    info.commandBufferCount = static_cast<std::uint32_t>(commands.size());
//    GX_VK_CHK_L(vkQueueSubmit(logical_device->get_graphic_queue(), 1, &info, nullptr))
//}

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

bool gearoenix::vulkan::engine::Engine::is_supported() noexcept
{
    Loader::load();
    return true;
}

#endif

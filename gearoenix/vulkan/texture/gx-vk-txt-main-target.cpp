#include "gx-vk-txt-main-target.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../system/gx-sys-application.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-framebuffer.hpp"
#include "../gx-vk-instance.hpp"
#include "../gx-vk-render-pass.hpp"
#include "../gx-vk-surface.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

gearoenix::vulkan::texture::MainTarget::Frame::Frame(
    std::shared_ptr<Framebuffer> framebuffer,
    std::shared_ptr<sync::Fence> wait_fence,
    std::shared_ptr<sync::Semaphore> start_semaphore,
    std::shared_ptr<sync::Semaphore> end_semaphore) noexcept
    : framebuffer(std::move(framebuffer))
    , wait_fence(std::move(wait_fence))
    , start_semaphore(std::move(start_semaphore))
    , end_semaphore(std::move(end_semaphore))
{
}

gearoenix::vulkan::texture::MainTarget::MainTarget(const MainTarget& o) noexcept
    : render::texture::Target(o)
    , swapchain(o.swapchain)
    , depth_stencil(o.depth_stencil)
    , render_pass(o.render_pass)
    , frames(o.frames)
{
}

gearoenix::vulkan::texture::MainTarget::MainTarget(memory::Manager& mem_mgr, engine::Engine* e) noexcept
    : render::texture::Target(core::asset::Manager::create_id(), e)
{
    const auto* const sys_app = e->get_system_application();
    clipping_width = static_cast<unsigned int>(sys_app->get_event_engine()->get_window_width());
    clipping_height = static_cast<unsigned int>(sys_app->get_event_engine()->get_window_height());
    const auto& logical_device = mem_mgr.get_logical_device();
    swapchain = std::make_shared<Swapchain>(logical_device);
    depth_stencil = image::View::create_depth_stencil(mem_mgr);
    render_pass = std::make_shared<RenderPass>(swapchain);
    const auto& frame_views = swapchain->get_image_views();
    frames.reserve(frame_views.size());
    for (const auto& frame_view : frame_views) {
        frames.emplace_back(
            std::make_shared<Framebuffer>(frame_view, depth_stencil, render_pass),
            std::make_shared<sync::Fence>(logical_device, true),
            std::make_shared<sync::Semaphore>(logical_device),
            std::make_shared<sync::Semaphore>(logical_device));
    }
}

gearoenix::vulkan::texture::MainTarget::~MainTarget() noexcept = default;

gearoenix::render::texture::Target* gearoenix::vulkan::texture::MainTarget::clone() const noexcept
{
    return new MainTarget(*this);
}

void gearoenix::vulkan::texture::MainTarget::update() noexcept
{
    current_frame = &frames[e->get_frame_number()];
    GX_CHECK_NOT_EQUAL_D(swapchain->get_next_image_index(current_frame->start_semaphore), 0xFFFFFFFF)
    current_frame->wait_fence->wait();
    current_frame->wait_fence->reset();
}

void gearoenix::vulkan::texture::MainTarget::present() noexcept
{
    auto vk_swap = swapchain->get_vulkan_data();
    const auto frame_number = static_cast<std::uint32_t>(e->get_frame_number());
    VkSemaphore sem = current_frame->end_semaphore->get_vulkan_data();
    VkPresentInfoKHR info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.swapchainCount = 1;
    info.pSwapchains = &vk_swap;
    info.pImageIndices = &frame_number;
    info.pWaitSemaphores = &sem;
    info.waitSemaphoreCount = 1;
    GX_VK_CHK_L(vkQueuePresentKHR(swapchain->get_logical_device()->get_graphic_queue(), &info))
}

#endif

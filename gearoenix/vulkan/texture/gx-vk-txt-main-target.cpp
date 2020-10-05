#include "gx-vk-txt-main-target.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../system/gx-sys-application.hpp"
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

gearoenix::vulkan::texture::MainTarget::MainTarget(const MainTarget& o) noexcept = default;

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
    const auto frames_count = frame_views.size();
    frames.reserve(frames_count);
    for (auto i = decltype(frames_count) { 0 }; i < frames_count; ++i) {
        frames.emplace_back(
            std::make_shared<Framebuffer>(frame_views[i], depth_stencil, render_pass),
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

#endif

#include "gx-vk-eng-frame.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-framebuffer.hpp"
#include "../gx-vk-swapchain.hpp"

gearoenix::vulkan::engine::Frame::Frame(
    std::shared_ptr<image::View>&& view, std::shared_ptr<image::View>&& depth, std::shared_ptr<RenderPass>&& render_pass)
    : framebuffer(new Framebuffer(std::move(view), std::move(depth), std::move(render_pass)))
{
}

gearoenix::vulkan::engine::Frame::~Frame() = default;

#endif
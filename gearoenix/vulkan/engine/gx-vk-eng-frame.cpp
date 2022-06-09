#include "gx-vk-eng-frame.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-framebuffer.hpp"
#include "../gx-vk-swapchain.hpp"

gearoenix::vulkan::engine::Frame::Frame(
    const Swapchain& swapchain,
    const image::View& depth_stencil,
    const RenderPass& render_pass,
    const unsigned int frame_index) noexcept
    : framebuffer(new Framebuffer(&swapchain.get_image_views()[frame_index], &depth_stencil, &render_pass))
{
}

gearoenix::vulkan::engine::Frame::~Frame() noexcept = default;

#endif
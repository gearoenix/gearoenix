#include "gx-vk-eng-frame.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED

gearoenix::vulkan::engine::Frame::Frame(
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

void gearoenix::vulkan::engine::Frame::begin() noexcept
{
    draw_wait.wait();
    draw_wait.reset();
    draw_command.begin();
}

#endif
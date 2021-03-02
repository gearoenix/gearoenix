#include "gx-vk-eng-frame.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../command/gx-vk-cmd-manager.hpp"
#include "../gx-vk-framebuffer.hpp"
#include "../gx-vk-swapchain.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "../sync/gx-vk-sync-semaphore.hpp"

gearoenix::vulkan::engine::Frame::Frame(
    command::Manager& command_manager,
    const Swapchain& swapchain,
    const image::View& depth_stencil,
    const RenderPass& render_pass,
    const unsigned int frame_index) noexcept
    : draw_command(new command::Buffer(std::move(command_manager.create(command::Type::Primary))))
    , draw_wait(new sync::Fence(command_manager.get_logical_device(), true))
    , framebuffer(new Framebuffer(&swapchain.get_image_views()[frame_index], &depth_stencil, &render_pass))
    , present_complete(new sync::Semaphore(command_manager.get_logical_device()))
    , render_complete(new sync::Semaphore(command_manager.get_logical_device()))
{
}

gearoenix::vulkan::engine::Frame::~Frame() noexcept = default;

void gearoenix::vulkan::engine::Frame::begin() noexcept
{
    draw_wait->wait();
    draw_wait->reset();
    draw_command->begin();
}

#endif
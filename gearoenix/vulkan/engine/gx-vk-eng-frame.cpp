#include "gx-vk-eng-frame.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-swapchain.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"

gearoenix::vulkan::engine::Frame::Frame(std::shared_ptr<image::View>&& view)
    : view(std::move(view))
{
}

gearoenix::vulkan::engine::Frame::~Frame() = default;

bool gearoenix::vulkan::engine::Frame::present()
{

}

#endif
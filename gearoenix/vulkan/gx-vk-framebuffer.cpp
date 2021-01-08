#include "gx-vk-framebuffer.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-render-pass.hpp"
#include "image/gx-vk-img-image.hpp"
#include "image/gx-vk-img-view.hpp"

gearoenix::vulkan::Framebuffer::Framebuffer(
    std::shared_ptr<image::View> v,
    std::shared_ptr<image::View> d,
    std::shared_ptr<RenderPass> rp) noexcept
    : view(std::move(v))
    , depth(std::move(d))
    , render_pass(std::move(rp))
{
    const auto& img = view->get_image();
    const auto& logical_device = img->get_logical_device();
    const VkImageView attachments[2] {
        view->get_vulkan_data(),
        depth->get_vulkan_data(),
    };
    VkFramebufferCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = render_pass->get_vulkan_data();
    info.layers = 1;
    info.attachmentCount = 2;
    info.pAttachments = attachments;
    info.width = img->get_image_width();
    info.height = img->get_image_height();
    GX_VK_CHK_L(vkCreateFramebuffer(logical_device->get_vulkan_data(), &info, nullptr, &vulkan_data))
}

gearoenix::vulkan::Framebuffer::~Framebuffer() noexcept
{
    const auto& logical_device = view->get_image()->get_logical_device();
    Loader::vkDestroyFramebuffer(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}
#endif

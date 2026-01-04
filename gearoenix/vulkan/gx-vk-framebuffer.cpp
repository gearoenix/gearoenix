#include "gx-vk-framebuffer.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-render-pass.hpp"
#include "image/gx-vk-img-view.hpp"

gearoenix::vulkan::Framebuffer::Framebuffer(Framebuffer&& o)
    : view(o.view)
    , depth(o.depth)
    , render_pass(o.render_pass)
    , vulkan_data(o.vulkan_data)
    , clear_colors(std::move(o.clear_colors))
{
    o.vulkan_data = nullptr;
}

gearoenix::vulkan::Framebuffer::Framebuffer(
    const image::View* const v,
    const image::View* const d,
    const RenderPass* const rp)
    : view(v)
    , depth(d)
    , render_pass(rp)
    , clear_colors {
        VkClearValue { .color = { 0.2f, 0.2f, 0.2f, 1.0f } },
        VkClearValue { .color = { 0.0f, 0.0f, 0.0f, 0.0f } }
    }
{
    const auto& img = view->get_image();
    const auto& logical_device = img.get_logical_device();
    const VkImageView attachments[2] {
        view->get_vulkan_data(),
        depth->get_vulkan_data(),
    };
    VkFramebufferCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = render_pass->get_vulkan_data();
    info.layers = 1;
    info.attachmentCount = 2;
    info.pAttachments = attachments;
    info.width = img.get_image_width();
    info.height = img.get_image_height();
    GX_VK_CHK(vkCreateFramebuffer(logical_device->get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::Framebuffer::~Framebuffer()
{
    if (nullptr != vulkan_data) {
        vkDestroyFramebuffer(view->get_image().get_logical_device()->get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

#endif

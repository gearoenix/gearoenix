#include "gx-vk-framebuffer.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-render-pass.hpp"
#include "image/gx-vk-img-view.hpp"

gearoenix::vulkan::Framebuffer::Framebuffer(
    std::shared_ptr<image::View>&& v,
    std::shared_ptr<image::View>&& d,
    std::shared_ptr<RenderPass>&& rp)
    : view(std::move(v))
    , depth(std::move(d))
    , render_pass(std::move(rp))
    , clear_colors {
        VkClearValue { .color = { 0.2f, 0.2f, 0.2f, 1.0f } },
        VkClearValue { .color = { 0.0f, 0.0f, 0.0f, 0.0f } }
    }
    , vulkan_data([this] {
        auto vk = decltype(vulkan_data){nullptr};
        const auto& img = view->get_image();
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
        info.width = img->get_image_width();
        info.height = img->get_image_height();
        GX_VK_CHK(vkCreateFramebuffer(device::Logical::get().get_vulkan_data(), &info, nullptr, &vk));

        return vk;
    }())
{
}

gearoenix::vulkan::Framebuffer::~Framebuffer()
{
    vkDestroyFramebuffer(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}

#endif

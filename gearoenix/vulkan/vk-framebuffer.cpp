#include "vk-framebuffer.hpp"
#include "vk-check.hpp"
#include "device/vk-dev-logical.hpp"
#include "device/vk-dev-physical.hpp"
#include "image/vk-img-image.hpp"
#include "image/vk-img-view.hpp"
#include "vk-render-pass.hpp"
#include "vk-instance.hpp"
#include "../core/static.hpp"

gearoenix::nufrag::render::Framebuffer::Framebuffer(
        const std::shared_ptr<image::View> &view,
        const std::shared_ptr<image::View> &depth,
        const std::shared_ptr<RenderPass> &render_pass) :
        view(view), depth(depth), render_pass(render_pass) {
    auto &d = view->get_image()->get_logical_device();
    auto &p = d->get_physical_device();
    auto &l = p->get_instance()->get_linker();
    auto cap = p->get_surface_capabilities();
    VkImageView attachments[2] = {
            view->get_vulkan_data(), depth->get_vulkan_data(),
    };
    VkFramebufferCreateInfo fb_create_info;
    setz(fb_create_info);
    fb_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_create_info.renderPass = render_pass->get_vulkan_data();
    fb_create_info.layers = 1;
    fb_create_info.attachmentCount = 2;
    fb_create_info.pAttachments = attachments;
    fb_create_info.width = cap->currentExtent.width;
    fb_create_info.height = cap->currentExtent.height;
    VKC(l->vkCreateFramebuffer(d->get_vulkan_data(), &fb_create_info, 0, &vulkan_data));
}

gearoenix::nufrag::render::Framebuffer::~Framebuffer() {
    auto &d = view->get_image()->get_logical_device();
    auto &p = d->get_physical_device();
    auto &l = p->get_instance()->get_linker();
    l->vkDestroyFramebuffer(d->get_vulkan_data(), vulkan_data, nullptr);
}

const VkFramebuffer &gearoenix::nufrag::render::Framebuffer::get_vulkan_data() const {
    return vulkan_data;
}

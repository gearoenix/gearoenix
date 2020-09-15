#include "gx-vk-framebuffer.hpp"
#ifdef USE_VULKAN
#include "../core/gx-cr-static.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-instance.hpp"
#include "gx-vk-render-pass.hpp"
#include "image/gx-vk-img-image.hpp"
#include "image/gx-vk-img-view.hpp"

gearoenix::render::Framebuffer::Framebuffer(image::View* view, image::View* depth, RenderPass* render_pass)
    : view(view)
    , depth(depth)
    , render_pass(render_pass)
{
    const image::Image* img = view->get_image();
    const device::Logical* d = img->get_logical_device();
    const device::Physical* p = d->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    const VkSurfaceCapabilitiesKHR& cap = p->get_surface_capabilities();
    VkImageView attachments[2] = {
        view->get_vulkan_data(),
        depth->get_vulkan_data(),
    };
    VkFramebufferCreateInfo fb_create_info;
    setz(fb_create_info);
    fb_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_create_info.renderPass = render_pass->get_vulkan_data();
    fb_create_info.layers = 1;
    fb_create_info.attachmentCount = 2;
    fb_create_info.pAttachments = attachments;
    fb_create_info.width = cap.currentExtent.width;
    fb_create_info.height = cap.currentExtent.height;
    VKC(l->vkCreateFramebuffer(d->get_vulkan_data(), &fb_create_info, 0, &vulkan_data));
}

gearoenix::render::Framebuffer::~Framebuffer()
{
    const image::Image* img = view->get_image();
    const device::Logical* d = img->get_logical_device();
    const device::Physical* p = d->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    l->vkDestroyFramebuffer(d->get_vulkan_data(), vulkan_data, nullptr);
}

const VkFramebuffer& gearoenix::render::Framebuffer::get_vulkan_data() const
{
    return vulkan_data;
}
#endif

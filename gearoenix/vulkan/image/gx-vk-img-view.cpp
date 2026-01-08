#include "gx-vk-img-view.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-flagger.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"

void gearoenix::vulkan::image::View::terminate()
{
    if (vulkan_data != nullptr) {
        vkDestroyImageView(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

gearoenix::vulkan::image::View::View(std::unique_ptr<Image>&& img)
    : image(std::move(img))
{
    VkImageViewCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = image->get_vulkan_data();
    info.viewType = image->get_image_depth() > 1 ? VK_IMAGE_VIEW_TYPE_3D : VK_IMAGE_VIEW_TYPE_2D;
    info.format = image->get_format();
    info.components.r = VK_COMPONENT_SWIZZLE_R;
    info.components.g = VK_COMPONENT_SWIZZLE_G;
    info.components.b = VK_COMPONENT_SWIZZLE_B;
    info.components.a = VK_COMPONENT_SWIZZLE_A;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.layerCount = 1;
    if GX_FLAG_CHECK (image->get_usage(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    } else {
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    GX_VK_CHK(vkCreateImageView(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::image::View::~View()
{
    terminate();
}

gearoenix::vulkan::image::View gearoenix::vulkan::image::View::create_depth_stencil()
{
    const auto& physical_device = device::Physical::get();
    const auto depth_format = physical_device.get_supported_depth_format();
    const auto surf_cap = physical_device.get_surface_capabilities();
    return View(std::make_unique<Image>(
        surf_cap.currentExtent.width, surf_cap.currentExtent.height,
        1, 1, 1, depth_format, 0,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
}

#endif

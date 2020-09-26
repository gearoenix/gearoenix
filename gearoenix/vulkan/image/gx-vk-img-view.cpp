#include "gx-vk-img-view.hpp"
#ifdef GX_USE_VULKAN
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "gx-vk-img-image.hpp"

gearoenix::vulkan::image::View::View(std::shared_ptr<Image> img) noexcept
    : image(std::move(img))
{
    VkImageViewCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = image->get_vulkan_data();
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = image->get_format();
    info.components.r = VK_COMPONENT_SWIZZLE_R;
    info.components.g = VK_COMPONENT_SWIZZLE_G;
    info.components.b = VK_COMPONENT_SWIZZLE_B;
    info.components.a = VK_COMPONENT_SWIZZLE_A;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.layerCount = 1;
    if (GX_FLAG_CHECK(image->get_usage(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    } else {
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    GX_VK_CHK_L(vkCreateImageView(
        image->get_logical_device()->get_vulkan_data(), &info, nullptr, &vulkan_data))
}

gearoenix::vulkan::image::View::~View() noexcept
{
    Loader::vkDestroyImageView(image->get_logical_device()->get_vulkan_data(), vulkan_data, nullptr);
}

std::shared_ptr<gearoenix::vulkan::image::View> gearoenix::vulkan::image::View::create_depth_stencil(
    memory::Manager& mem_mgr) noexcept
{
    const auto& physical_device = mem_mgr.get_logical_device()->get_physical_device();
    const VkFormat depth_format = physical_device->get_supported_depth_format();
    const VkSurfaceCapabilitiesKHR& surf_cap = physical_device->get_surface_capabilities();
    return std::make_shared<View>(std::make_shared<Image>(
        surf_cap.currentExtent.width,
        surf_cap.currentExtent.height,
        1, 1, 1,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        depth_format, mem_mgr));
}
#endif

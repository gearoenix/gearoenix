#include "gx-vk-img-view.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-instance.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "gx-vk-img-image.hpp"
//
//gearoenix::vulkan::image::View::View(Image* img, const VkFormat& format)
//    : img(img)
//{
//    const device::Logical* d = img->get_logical_device();
//    const Linker* l = d->get_physical_device()->get_instance()->get_linker();
//    VkImageViewCreateInfo view_create_info;
//    setz(view_create_info);
//    view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//    view_create_info.image = img->get_vulkan_data();
//    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
//    view_create_info.format = format;
//    view_create_info.components.r = VK_COMPONENT_SWIZZLE_R;
//    view_create_info.components.g = VK_COMPONENT_SWIZZLE_G;
//    view_create_info.components.b = VK_COMPONENT_SWIZZLE_B;
//    view_create_info.components.a = VK_COMPONENT_SWIZZLE_A;
//    view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    view_create_info.subresourceRange.levelCount = 1;
//    view_create_info.subresourceRange.layerCount = 1;
//    VKC(l->vkCreateImageView(d->get_vulkan_data(), &view_create_info, 0, &vulkan_data));
//}
//
//gearoenix::vulkan::image::View::View(Image* img,
//    const VkImageViewCreateInfo& info)
//    : img(img)
//{
//    const device::Logical* d = img->get_logical_device();
//    const Linker* l = d->get_physical_device()->get_instance()->get_linker();
//    const VkDevice vk_dev = d->get_vulkan_data();
//    VKC(l->vkCreateImageView(vk_dev, &info, 0, &vulkan_data));
//}
//
//gearoenix::vulkan::image::View::~View()
//{
//    const device::Logical* d = img->get_logical_device();
//    const Linker* l = d->get_physical_device()->get_instance()->get_linker();
//    const VkDevice vk_dev = d->get_vulkan_data();
//    l->vkDestroyImageView(vk_dev, vulkan_data, 0);
//    delete img;
//}
//
std::shared_ptr<gearoenix::vulkan::image::View> gearoenix::vulkan::image::View::create_depth_stencil(memory::Manager& mem_mgr) noexcept
{
    auto& physical_device = *mem_mgr.get_logical_device()->get_physical_device();
    const VkFormat depth_format = physical_device.get_supported_depth_format();
    const VkSurfaceCapabilitiesKHR& surf_cap = physical_device.get_surface_capabilities();
    VkImageCreateInfo image_info;
    GX_SET_ZERO(image_info);
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = depth_format;
    image_info.extent.width = surf_cap.currentExtent.width;
    image_info.extent.height = surf_cap.currentExtent.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    Image* img = new Image(d, image_info);
    VkImageViewCreateInfo depth_stencil_view_info;
    setz(depth_stencil_view_info);
    depth_stencil_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depth_stencil_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depth_stencil_view_info.format = depth_format;
    depth_stencil_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depth_stencil_view_info.subresourceRange.levelCount = 1;
    depth_stencil_view_info.subresourceRange.layerCount = 1;
    depth_stencil_view_info.image = img->get_vulkan_data();
    return new View(img, depth_stencil_view_info);
}
#endif

#include "gx-vk-swapchain.hpp"
#ifdef GX_USE_VULKAN
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-instance.hpp"
#include "gx-vk-surface.hpp"
#include "image/gx-vk-img-image.hpp"
#include "image/gx-vk-img-view.hpp"
#include "sync/gx-vk-sync-semaphore.hpp"

#ifdef GX_DEBUG_MODE
#define GX_DEBUG_SWAPCHAIN
#endif

gearoenix::vulkan::Swapchain::Swapchain(std::shared_ptr<device::Logical> d) noexcept
    : logical_device(std::move(d))
    , format {}
{
    initialize();
}

gearoenix::vulkan::Swapchain::~Swapchain() noexcept
{
    image_views.clear();
    Loader::vkDestroySwapchainKHR(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}

std::uint32_t gearoenix::vulkan::Swapchain::get_next_image_index(const std::shared_ptr<sync::Semaphore>& semaphore) noexcept
{
    std::uint32_t image_index = 0;
    const VkResult result = Loader::vkAcquireNextImageKHR(
        logical_device->get_vulkan_data(), vulkan_data,
        std::numeric_limits<std::uint64_t>::max(),
        semaphore->get_vulkan_data(), nullptr, &image_index);
    switch (result) {
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_ERROR_INITIALIZATION_FAILED:
        return 0xffffffff;
    case VK_SUCCESS:
        return image_index;
    default:
        GXLOGF("Swapchain failed to get the next image, result: " << result_to_string(result))
    }
}

void gearoenix::vulkan::Swapchain::initialize() noexcept
{
    const auto& physical_device = logical_device->get_physical_device();
    const auto& surface = physical_device->get_surface();
    const VkSurfaceCapabilitiesKHR& caps = physical_device->get_surface_capabilities();
    const auto& formats = physical_device->get_surface_formats();
    const auto old_swapchain = vulkan_data;
    std::uint32_t chosen_format_index;
    for (chosen_format_index = 0; chosen_format_index < formats.size(); ++chosen_format_index) {
        if (formats[chosen_format_index].format == VK_FORMAT_R8G8B8A8_UNORM)
            break;
    }
    if (chosen_format_index >= formats.size()) {
        GXLOGD("VK_FORMAT_R8G8B8A8_UNORM not found in the surface.")
        chosen_format_index = 0;
    }
    format = formats[chosen_format_index];
    auto swapchain_images_count = caps.minImageCount + 1;
    if (swapchain_images_count < 1)
        swapchain_images_count = 1;
    if ((caps.maxImageCount > 0) && (swapchain_images_count > caps.maxImageCount)) {
        swapchain_images_count = caps.maxImageCount;
    }
    std::uint32_t image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkFormatProperties format_props;
    Loader::vkGetPhysicalDeviceFormatProperties(physical_device->get_vulkan_data(), format.format, &format_props);
    if (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) {
        image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    VkSwapchainCreateInfoKHR swapchain_create_info;
    GX_SET_ZERO(swapchain_create_info)
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = surface->get_vulkan_data();
    swapchain_create_info.minImageCount = swapchain_images_count;
    swapchain_create_info.imageFormat = format.format;
    swapchain_create_info.imageColorSpace = format.colorSpace;
    swapchain_create_info.imageExtent = caps.currentExtent;
    swapchain_create_info.imageUsage = image_usage;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchain_create_info.oldSwapchain = old_swapchain;
    swapchain_create_info.clipped = VK_TRUE;
    if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    } else if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    } else if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    } else if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
    } else if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
    } else {
        GXLOGF("Error composite is unknown.")
    }
    GX_VK_CHK_L(vkCreateSwapchainKHR(logical_device->get_vulkan_data(), &swapchain_create_info, nullptr, &vulkan_data))
    std::uint32_t count = 0;
    GX_VK_CHK_L(vkGetSwapchainImagesKHR(logical_device->get_vulkan_data(), vulkan_data, &count, nullptr))
    std::vector<VkImage> images(count);
    GX_VK_CHK_L(vkGetSwapchainImagesKHR(logical_device->get_vulkan_data(), vulkan_data, &count, images.data()))
    image_views.reserve(static_cast<std::size_t>(count));
    for (uint32_t i = 0; i < count; ++i) {
        image_views.emplace_back(new image::View(std::make_shared<image::Image>(logical_device, images[i])));
    }
    if (nullptr != old_swapchain) {
        Loader::vkDestroySwapchainKHR(logical_device->get_vulkan_data(), old_swapchain, nullptr);
    }
}
#endif

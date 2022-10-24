#include "gx-vk-swapchain.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "engine/gx-vk-eng-engine.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-instance.hpp"
#include "gx-vk-surface.hpp"
#include "sync/gx-vk-sync-semaphore.hpp"

#ifdef GX_DEBUG_MODE
#define GX_DEBUG_SWAPCHAIN
#endif

gearoenix::vulkan::Swapchain::Swapchain(const engine::Engine& e) noexcept
    : e(e)
    , logical_device(e.get_logical_device())
    , format {}
{
    initialize();
}

gearoenix::vulkan::Swapchain::~Swapchain() noexcept
{
    vkDestroySwapchainKHR(logical_device.get_vulkan_data(), vulkan_data, nullptr);
}

std::optional<std::uint32_t> gearoenix::vulkan::Swapchain::get_next_image_index(const sync::Semaphore& semaphore) noexcept
{
    std::uint32_t image_index = 0;
    const VkResult result = vkAcquireNextImageKHR(
        logical_device.get_vulkan_data(), vulkan_data,
        std::numeric_limits<std::uint64_t>::max(),
        semaphore.get_vulkan_data(), nullptr, &image_index);
    switch (result) {
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_ERROR_INITIALIZATION_FAILED:
        return std::nullopt;
    case VK_SUCCESS:
        return image_index;
    default:
        GX_LOG_F("Swapchain failed to get the next image, result: " << result_to_string(result));
    }
}

void gearoenix::vulkan::Swapchain::initialize() noexcept
{
    image_views.clear();
    const auto& physical_device = logical_device.get_physical_device();
    const auto& surface = physical_device.get_surface();
    const auto caps = physical_device.get_surface_capabilities();
    const auto& formats = physical_device.get_surface_formats();
    const auto old_swapchain = vulkan_data;
    std::uint32_t chosen_format_index;
    for (chosen_format_index = 0; chosen_format_index < formats.size(); ++chosen_format_index) {
        if (formats[chosen_format_index].format == VK_FORMAT_R8G8B8A8_UNORM)
            break;
    }
    if (chosen_format_index >= formats.size()) {
        GX_LOG_D("VK_FORMAT_R8G8B8A8_UNORM not found in the surface.");
        chosen_format_index = 0;
    }
    format = formats[chosen_format_index];
    std::uint32_t swapchain_images_count = caps.maxImageCount > 2 ? 3 : 2;
    if ((caps.maxImageCount > 0) && (swapchain_images_count > caps.maxImageCount)) {
        swapchain_images_count = caps.maxImageCount;
    }
    std::uint32_t image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkFormatProperties format_props;
    vkGetPhysicalDeviceFormatProperties(physical_device.get_vulkan_data(), format.format, &format_props);
    if (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) {
        image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    VkSwapchainCreateInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface.get_vulkan_data();
    info.minImageCount = swapchain_images_count;
    info.imageFormat = format.format;
    info.imageColorSpace = format.colorSpace;
    if (caps.currentExtent.width != static_cast<decltype(caps.currentExtent.width)>(-1) && caps.currentExtent.width != 0 && caps.currentExtent.height != static_cast<decltype(caps.currentExtent.height)>(-1) && caps.currentExtent.height != 0) {
        info.imageExtent = caps.currentExtent;
    } else {
        const auto window_size = e.get_platform_application().get_base().get_window_size();
        info.imageExtent.width = static_cast<decltype(caps.currentExtent.width)>(window_size.x);
        info.imageExtent.height = static_cast<decltype(caps.currentExtent.height)>(window_size.y);
    }
    info.imageUsage = image_usage;
    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.imageArrayLayers = 1;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    info.oldSwapchain = old_swapchain;
    info.clipped = VK_TRUE;
    if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) != 0) {
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    } else if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) != 0) {
        info.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    } else if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) != 0) {
        info.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    } else if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) != 0) {
        info.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
    } else if ((caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR) != 0) {
        info.compositeAlpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
    } else {
        GX_LOG_F("Error composite is unknown.");
    }
    GX_VK_CHK(vkCreateSwapchainKHR(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data));
    std::uint32_t count = 0;
    GX_VK_CHK(vkGetSwapchainImagesKHR(logical_device.get_vulkan_data(), vulkan_data, &count, nullptr));
    std::vector<VkImage> images(count);
    GX_VK_CHK(vkGetSwapchainImagesKHR(logical_device.get_vulkan_data(), vulkan_data, &count, images.data()));
    image_views.reserve(static_cast<std::size_t>(count));
    for (uint32_t i = 0; i < count; ++i) {
        image_views.emplace_back(image::Image(
            &logical_device, info.imageExtent.width, info.imageExtent.height, 1, 1, 1,
            info.imageFormat, 0, info.imageUsage, images[i]));
    }
    if (nullptr != old_swapchain) {
        vkDestroySwapchainKHR(logical_device.get_vulkan_data(), old_swapchain, nullptr);
    }
}

const VkSwapchainKHR* gearoenix::vulkan::Swapchain::get_vulkan_data_ptr() const noexcept
{
    return &vulkan_data;
}

#endif

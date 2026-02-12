#include "gx-vk-swapchain.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/gx-plt-application.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "engine/gx-vk-eng-engine.hpp"
#include "gx-vk-check.hpp"
#include "gx-vk-surface.hpp"
#include "image/gx-vk-img-image.hpp"
#include "image/gx-vk-img-view.hpp"
#include "queue/gx-vk-qu-queue.hpp"
#include "sync/gx-vk-sync-semaphore.hpp"

#include <utility>

#if GX_DEBUG_MODE
#define GX_DEBUG_SWAPCHAIN true
#endif

namespace {
[[nodiscard]] std::optional<VkFormat> srgb_to_unorm(const VkFormat format)
{
    switch (format) {
    case VK_FORMAT_R8G8B8A8_SRGB:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return VK_FORMAT_B8G8R8A8_UNORM;
    default:
        return std::nullopt;
    }
}
}

gearoenix::vulkan::Swapchain::Swapchain()
    : Singleton(this)
    , format {}
{
    initialize();
}

gearoenix::vulkan::Swapchain::~Swapchain()
{
    vkDestroySwapchainKHR(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}

void gearoenix::vulkan::Swapchain::acquire_next_image(const sync::Semaphore& semaphore)
{
    const VkResult result = vkAcquireNextImageKHR(
        device::Logical::get().get_vulkan_data(),
        vulkan_data,
        std::numeric_limits<std::uint64_t>::max(),
        semaphore.get_vulkan_data(),
        nullptr,
        &image_index);
    switch (result) {
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_ERROR_INITIALIZATION_FAILED:
        is_valid = false;
        return;
    case VK_SUCCESS:
        is_valid = true;
        return;
    default:
        GX_LOG_F("Swapchain failed to get the next image, result: " << result_to_string(result));
    }
}

void gearoenix::vulkan::Swapchain::initialize()
{
    frames = {};

    const auto& physical_device = device::Physical::get();
    const auto& logical_device = device::Logical::get();
    const auto& surface = Surface::get();
    const auto caps = physical_device.get_surface_capabilities();
    const auto& formats = physical_device.get_surface_formats();
    const auto old_swapchain = vulkan_data;

    auto chosen_format_index = [&] {
        constexpr std::array acceptable_formats {
            std::pair { VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
            std::pair { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
            std::pair { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
            std::pair { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
        };
        for (const auto& acceptable_format : acceptable_formats) {
            for (std::uint32_t i = 0; i < formats.size(); ++i) {
                if (formats[i].format == acceptable_format.first && formats[i].colorSpace == acceptable_format.second) {
                    return i;
                }
            }
        }
        return std::numeric_limits<std::uint32_t>::max();
    }();

    if (chosen_format_index >= formats.size()) {
        GX_LOG_D("The required format not found in the surface.");
        chosen_format_index = 0;
    }

    format = formats[chosen_format_index];
    GX_ASSERT_D(caps.minImageCount <= frames_in_flight);
    GX_ASSERT_D(caps.maxImageCount == 0 || caps.maxImageCount >= frames_in_flight);

    std::uint32_t image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkFormatProperties format_props;
    vkGetPhysicalDeviceFormatProperties(physical_device.get_vulkan_data(), format.format, &format_props);
    // Enable a transfer source if supported (for screenshots, etc.)
    if ((caps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0
        && (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT) != 0) {
        image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    // Enable transfer destination if supported (for blitting camera render targets to swapchain)
    if ((caps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) != 0
        && (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT) != 0) {
        image_usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    const auto unorm_format = srgb_to_unorm(format.format);
    const auto mutable_format_supported = unorm_format.has_value()
        && physical_device.get_supported_extensions().contains(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME);

    // For mutable format: list both sRGB and UNORM so we can create a UNORM view for ImGui
    std::array<VkFormat, 2> format_list_entries {};
    VkImageFormatListCreateInfo format_list_info;
    GX_SET_ZERO(format_list_info);
    format_list_info.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;

    if (mutable_format_supported) {
        format_list_entries = { format.format, *unorm_format };
        format_list_info.viewFormatCount = static_cast<std::uint32_t>(format_list_entries.size());
        format_list_info.pViewFormats = format_list_entries.data();
    }

    VkSwapchainCreateInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    if (mutable_format_supported) {
        info.flags = VK_SWAPCHAIN_CREATE_MUTABLE_FORMAT_BIT_KHR;
        info.pNext = &format_list_info;
    }
    info.surface = surface.get_vulkan_data();
    info.minImageCount = frames_in_flight;
    info.imageFormat = format.format;
    info.imageColorSpace = format.colorSpace;
    if (caps.currentExtent.width != std::numeric_limits<decltype(caps.currentExtent.width)>::max() && caps.currentExtent.width != 0 && caps.currentExtent.height != std::numeric_limits<decltype(caps.currentExtent.height)>::max() && caps.currentExtent.height != 0) {
        info.imageExtent = caps.currentExtent;
    } else {
        const auto window_size = platform::BaseApplication::get().get_window_size();
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
    } else {
        const auto fallback = static_cast<VkCompositeAlphaFlagBitsKHR>(caps.supportedCompositeAlpha & ~(caps.supportedCompositeAlpha - 1));
        if (fallback == 0) {
            GX_LOG_F("Error composite is unknown.");
        }
        info.compositeAlpha = fallback;
    }
    GX_VK_CHK(vkCreateSwapchainKHR(logical_device.get_vulkan_data(), &info, nullptr, &vulkan_data));
    std::uint32_t count = 0;
    GX_VK_CHK(vkGetSwapchainImagesKHR(logical_device.get_vulkan_data(), vulkan_data, &count, nullptr));
    std::vector<VkImage> images(count);
    GX_VK_CHK(vkGetSwapchainImagesKHR(logical_device.get_vulkan_data(), vulkan_data, &count, images.data()));

    for (auto i = 0; i < count; ++i) {
        auto img = std::make_shared<image::Image>(
            "swapchain-img-" + std::to_string(i),
            static_cast<std::uint32_t>(info.imageExtent.width),
            static_cast<std::uint32_t>(info.imageExtent.height),
            static_cast<std::uint32_t>(1),
            VK_IMAGE_TYPE_2D,
            static_cast<std::uint32_t>(1),
            static_cast<std::uint32_t>(1),
            info.imageFormat,
            mutable_format_supported ? VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT : static_cast<VkImageCreateFlags>(0),
            info.imageUsage,
            images[i]);
        img->set_owned(false);
        frames[i].view = std::make_shared<image::View>(std::shared_ptr(img));
        if (mutable_format_supported) {
            frames[i].imgui_view = std::make_shared<image::View>(
                std::shared_ptr(img), 0, std::nullopt, 0, std::nullopt, *unorm_format);
        }
        frames[i].present = std::make_unique<sync::Semaphore>("present-" + std::to_string(i));
    }
    if (nullptr != old_swapchain) {
        vkDestroySwapchainKHR(logical_device.get_vulkan_data(), old_swapchain, nullptr);
    }
    is_valid = true;
    image_index = 0;
}

const VkSwapchainKHR* gearoenix::vulkan::Swapchain::get_vulkan_data_ptr() const
{
    return &vulkan_data;
}

void gearoenix::vulkan::Swapchain::present()
{
    VkPresentInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = frames[image_index].present->get_vulkan_data_ptr();
    info.swapchainCount = 1;
    info.pSwapchains = &vulkan_data;
    info.pImageIndices = &image_index;
    const auto present_result = queue::Queue::get().present(info);
    if (VK_ERROR_OUT_OF_DATE_KHR == present_result) {
        is_valid = false;
        return;
    }
    if (VK_SUCCESS != present_result) {
        GX_LOG_F("Presentation failed with result: " << result_to_string(present_result));
    }
    is_valid = true;
}

const gearoenix::vulkan::sync::Semaphore& gearoenix::vulkan::Swapchain::get_present_semaphore() const
{
    return *frames[image_index].present;
}

#endif

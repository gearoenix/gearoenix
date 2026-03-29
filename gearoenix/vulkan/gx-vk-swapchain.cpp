#include "gx-vk-swapchain.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../platform/gx-plt-application.hpp"
#include "device/gx-vk-dev-logical.hpp"
#include "device/gx-vk-dev-physical.hpp"
#include "engine/gx-vk-eng-engine.hpp"
#include "gx-vk-surface.hpp"
#include "image/gx-vk-img-image.hpp"
#include "image/gx-vk-img-view.hpp"
#include "queue/gx-vk-qu-queue.hpp"
#include "sync/gx-vk-sync-semaphore.hpp"

#include <utility>

namespace {
[[nodiscard]] std::optional<vk::Format> srgb_to_unorm(const vk::Format format)
{
    switch (format) {
    case vk::Format::eR8G8B8A8Srgb:
        return vk::Format::eR8G8B8A8Unorm;
    case vk::Format::eB8G8R8A8Srgb:
        return vk::Format::eB8G8R8A8Unorm;
    default:
        return std::nullopt;
    }
}
}

gearoenix::vulkan::Swapchain::Swapchain()
    : Singleton(this)
{
    initialize();
}

gearoenix::vulkan::Swapchain::~Swapchain() = default;

void gearoenix::vulkan::Swapchain::acquire_next_image(const sync::Semaphore& semaphore)
{
    try {
        auto [result, index] = device::Logical::get().get_vulkan_data().acquireNextImageKHR(
            *vulkan_data,
            std::numeric_limits<std::uint64_t>::max(),
            semaphore.get_vulkan_data());
        image_index = index;
        is_valid = true;
    } catch (const vk::OutOfDateKHRError&) {
        is_valid = false;
    } catch (const vk::InitializationFailedError&) {
        is_valid = false;
    } catch (const vk::SystemError& e) {
        GX_LOG_F("Swapchain failed to get the next image: " << e.what());
    }
}

void gearoenix::vulkan::Swapchain::initialize()
{
    frames = { };

    const auto& physical_device = device::Physical::get();
    const auto& logical_device = device::Logical::get();
    const auto& surface = Surface::get();
    const auto caps = physical_device.get_surface_capabilities();
    const auto& formats = physical_device.get_surface_formats();
    vulkan_data = vk::raii::SwapchainKHR { nullptr };

    auto chosen_format_index = [&] {
        constexpr std::array acceptable_formats {
            std::pair { vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear },
            std::pair { vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear },
            std::pair { vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear },
            std::pair { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear },
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

    vk::ImageUsageFlags image_usage = vk::ImageUsageFlagBits::eColorAttachment;
    const auto format_props = physical_device.get_vulkan_data().getFormatProperties(format.format);
    // Enable a transfer source if supported (for screenshots, etc.)
    if ((caps.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferSrc) && (format_props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eTransferSrc)) {
        image_usage |= vk::ImageUsageFlagBits::eTransferSrc;
    }
    // Enable transfer destination if supported (for blitting camera render targets to swapchain)
    if ((caps.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferDst) && (format_props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eTransferDst)) {
        image_usage |= vk::ImageUsageFlagBits::eTransferDst;
    }

    const auto unorm_format = srgb_to_unorm(format.format);
    const auto mutable_format_supported = unorm_format.has_value() && physical_device.get_supported_extensions().contains(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME);

    // For mutable format: list both sRGB and UNORM so we can create a UNORM view for ImGui
    vk::ImageFormatListCreateInfo format_list_info;
    std::array format_list_entries = { format.format, unorm_format.value_or(format.format) };

    if (mutable_format_supported) {
        format_list_info.setViewFormats(format_list_entries);
    }

    vk::SwapchainCreateInfoKHR info;
    if (mutable_format_supported) {
        info.flags = vk::SwapchainCreateFlagBitsKHR::eMutableFormat;
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
    info.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    info.imageArrayLayers = 1;
    info.imageSharingMode = vk::SharingMode::eExclusive;
    info.presentMode = vk::PresentModeKHR::eFifo;
    info.clipped = true;
    if (caps.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque) {
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    } else if (caps.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) {
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eInherit;
    } else if (caps.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
    } else if (caps.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePostMultiplied;
    } else {
        GX_LOG_F("Error composite is unknown.");
    }

    const auto dev = logical_device.get_vulkan_data();
    vulkan_data = vk::raii::SwapchainKHR(logical_device.get_device(), info);
    auto images = dev.getSwapchainImagesKHR(*vulkan_data);
    GX_ASSERT_D(images.size() == frames_in_flight);

    for (std::uint32_t i = 0; i < images.size(); ++i) {
        auto img = std::make_shared<image::Image>(
            "swapchain-img-" + std::to_string(i),
            static_cast<std::uint32_t>(info.imageExtent.width),
            static_cast<std::uint32_t>(info.imageExtent.height),
            static_cast<std::uint32_t>(1),
            vk::ImageType::e2D,
            static_cast<std::uint32_t>(1),
            static_cast<std::uint32_t>(1),
            info.imageFormat,
            mutable_format_supported ? vk::ImageCreateFlagBits::eMutableFormat : vk::ImageCreateFlags { },
            info.imageUsage,
            images[i]);
        img->set_owned(false);
        frames[i].view = std::make_shared<image::View>(std::shared_ptr(img));
        if (mutable_format_supported) {
            frames[i].imgui_view = std::make_shared<image::View>(std::shared_ptr(img), 0, std::nullopt, 0, std::nullopt, *unorm_format);
        }
        frames[i].present = std::make_unique<sync::Semaphore>("present-" + std::to_string(i));
    }
    is_valid = true;
    image_index = 0;
}

void gearoenix::vulkan::Swapchain::present()
{
    const auto swapchain_handle = *vulkan_data;
    const auto present_semaphore = frames[image_index].present->get_vulkan_data();
    vk::PresentInfoKHR info;
    info.setWaitSemaphores(present_semaphore);
    info.setSwapchains(swapchain_handle);
    info.pImageIndices = &image_index;
    try {
        queue::Queue::get().present(info);
        is_valid = true;
    } catch (const vk::OutOfDateKHRError&) {
        is_valid = false;
    } catch (const vk::SystemError& e) {
        GX_LOG_F("Presentation failed: " << e.what());
    }
}

const gearoenix::vulkan::sync::Semaphore& gearoenix::vulkan::Swapchain::get_present_semaphore() const
{
    return *frames[image_index].present;
}

#endif

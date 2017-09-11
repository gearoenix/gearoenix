#include "vk-swapchain.hpp"
#include "vk-check.hpp"
#include "vk-instance.hpp"
#include "vk-surface.hpp"
#include "device/vk-dev-logical.hpp"
#include "device/vk-dev-physical.hpp"
#include "image/vk-img-image.hpp"
#include "image/vk-img-view.hpp"
#include "sync/vk-sync-semaphore.hpp"
#include "../core/static.hpp"

#define DEBUG_SWAPCHAIN

gearoenix::nufrag::render::Swapchain::Swapchain(const std::shared_ptr<device::Logical> &d)
    : logical_device(d) {
    initialize();
}

gearoenix::nufrag::render::Swapchain::~Swapchain() {
    image_views.clear();
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    l->vkDestroySwapchainKHR(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}

const VkSwapchainKHR &gearoenix::nufrag::render::Swapchain::get_vulkan_data() const {
    return vulkan_data;
}

const VkSurfaceFormatKHR &gearoenix::nufrag::render::Swapchain::get_chosen_format() const {
    return chosen_format;
}

std::vector<std::shared_ptr<gearoenix::nufrag::render::image::View> >
gearoenix::nufrag::render::Swapchain::get_image_views() const {
    return image_views;
}

const std::shared_ptr<gearoenix::nufrag::render::device::Logical> &
gearoenix::nufrag::render::Swapchain::get_logical_device() const {
    return logical_device;
}


uint32_t gearoenix::nufrag::render::Swapchain::get_next_image_index(const std::shared_ptr<sync::Semaphore> &semaphore) {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    uint32_t image_index = 0;
    VkResult r = l->vkAcquireNextImageKHR(logical_device->get_vulkan_data(), vulkan_data, UINT64_MAX, semaphore->get_vulkan_data(), nullptr, &image_index);
    if(r == VK_ERROR_OUT_OF_DATE_KHR || r == VK_ERROR_INITIALIZATION_FAILED) {
        return 0xffffffff;
//        initialize();
    } else {
        VKC(r);
    }
    return image_index;
}

void gearoenix::nufrag::render::Swapchain::initialize() {
    auto &d = logical_device;
    auto &p = d->get_physical_device();
    auto &s = p->get_surface();
    auto &i = p->get_instance();
    auto &l = i->get_linker();
    auto caps = p->get_surface_capabilities();
    auto formats = p->get_surface_formats();
    auto old_swapchain = vulkan_data;
    uint32_t chosen_format_index;
    for (chosen_format_index = 0; chosen_format_index < formats.size(); chosen_format_index++) {
        if (formats[chosen_format_index].format == VK_FORMAT_R8G8B8A8_UNORM)
            break;
    }
    if (chosen_format_index >= formats.size()) {
        LOGD(std::string("VK_FORMAT_R8G8B8A8_UNORM not found in the surface."));
        chosen_format_index = 0;
    }
    chosen_format = formats[chosen_format_index];
    auto swapchain_images_count = caps->minImageCount + 1;
    if ((caps->maxImageCount > 0) && (swapchain_images_count > caps->maxImageCount)) {
        swapchain_images_count = caps->maxImageCount;
    }
    uint32_t image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkFormatProperties format_props;
    l->vkGetPhysicalDeviceFormatProperties(p->get_vulkan_data(), chosen_format.format,
                                           &format_props);
    if (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) {
        image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    VkSwapchainCreateInfoKHR swapchain_create_info;
    setz(swapchain_create_info);
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = s->get_vulkan_data();
    swapchain_create_info.minImageCount = swapchain_images_count;
    swapchain_create_info.imageFormat = chosen_format.format;
    swapchain_create_info.imageColorSpace = chosen_format.colorSpace;
    swapchain_create_info.imageExtent = caps->currentExtent;
    swapchain_create_info.imageUsage = image_usage;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchain_create_info.oldSwapchain = old_swapchain;
    swapchain_create_info.clipped = VK_TRUE;
    if ((caps->supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    } else if ((caps->supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    } else if ((caps->supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    } else if ((caps->supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
    } else if ((caps->supportedCompositeAlpha & VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR) != 0) {
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
    } else {
        LOGF(std::string("Error composite is unknown."));
    }
    VKC(l->vkCreateSwapchainKHR(d->get_vulkan_data(), &swapchain_create_info, 0, &vulkan_data));
    uint32_t count = 0;
    VKC(l->vkGetSwapchainImagesKHR(d->get_vulkan_data(), vulkan_data, &count, nullptr));
    std::vector<VkImage> images(count);
    VKC(l->vkGetSwapchainImagesKHR(d->get_vulkan_data(), vulkan_data, &count, images.data()));
    image_views.resize(count);
    for (uint32_t i = 0; i < count; ++i) {
        image_views[i] = std::shared_ptr<image::View>(
                    new image::View(std::shared_ptr<image::Image>(new image::Image(d, images[i])),
                                    chosen_format.format));
    }
    if(0 != old_swapchain) {
        l->vkDestroySwapchainKHR(logical_device->get_vulkan_data(), old_swapchain, nullptr);
    }
}

#include "gx-vk-img-view.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-flagger.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"

namespace {
[[nodiscard]] VkImageAspectFlags get_depth_stencil_aspect(const VkFormat format)
{
    switch (format) {
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case VK_FORMAT_S8_UINT:
        return VK_IMAGE_ASPECT_STENCIL_BIT;
    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    default:
        GX_UNEXPECTED;
    }
}
}

gearoenix::vulkan::image::View::View(
    std::shared_ptr<Image>&& img,
    std::uint32_t base_level,
    std::optional<std::uint32_t> level_count,
    std::uint32_t base_layer,
    std::optional<std::uint32_t> layer_count)
    : image(std::move(img))
    , extent {
        .width = std::max(image->get_image_width() >> base_level, 1u),
        .height = std::max(image->get_image_height() >> base_level, 1u),
        .depth = std::max(image->get_image_depth() >> base_level, 1u),
    }
    , base_level(base_level)
    , level_count(level_count.value_or(image->get_mipmap_levels() - base_level))
    , base_layer(base_layer)
    , layer_count(layer_count.value_or(image->get_array_layers() - base_layer))
{
    GX_ASSERT_D(nullptr != image);
    GX_ASSERT_D(base_level < image->get_mipmap_levels());
    GX_ASSERT_D(base_level + this->level_count <= image->get_mipmap_levels());
    GX_ASSERT_D(base_layer < image->get_array_layers());
    GX_ASSERT_D(base_layer + this->layer_count <= image->get_array_layers());

    VkImageViewCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = image->get_vulkan_data();

    const bool is_cube = GX_FLAG_CHECK(image->get_flags(), VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) && this->layer_count >= 6 && this->layer_count % 6 == 0;

    switch (image->get_image_type()) {
    case VK_IMAGE_TYPE_1D:
        info.viewType = this->layer_count > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
        break;
    case VK_IMAGE_TYPE_2D:
        if (is_cube) {
            info.viewType = this->layer_count > 6 ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
        } else {
            info.viewType = this->layer_count > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
        }
        break;
    case VK_IMAGE_TYPE_3D:
        info.viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
    default:
        GX_UNEXPECTED;
    }

    info.format = image->get_format();
    info.components.r = VK_COMPONENT_SWIZZLE_R;
    info.components.g = VK_COMPONENT_SWIZZLE_G;
    info.components.b = VK_COMPONENT_SWIZZLE_B;
    info.components.a = VK_COMPONENT_SWIZZLE_A;
    info.subresourceRange.baseMipLevel = this->base_level;
    info.subresourceRange.levelCount = this->level_count;
    info.subresourceRange.baseArrayLayer = this->base_layer;
    info.subresourceRange.layerCount = this->layer_count;

    if GX_FLAG_CHECK (image->get_usage(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        info.subresourceRange.aspectMask = get_depth_stencil_aspect(image->get_format());
    } else {
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    GX_VK_CHK(vkCreateImageView(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::image::View::~View()
{
    vkDestroyImageView(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}

std::shared_ptr<gearoenix::vulkan::image::View> gearoenix::vulkan::image::View::create_depth_stencil(const std::string& name)
{
    const auto& physical_device = device::Physical::get();
    const auto depth_format = physical_device.get_supported_depth_format();
    const auto surf_cap = physical_device.get_surface_capabilities();
    return std::make_shared<View>(std::make_shared<Image>(
        name, surf_cap.currentExtent.width, surf_cap.currentExtent.height, 1, VK_IMAGE_TYPE_2D, 1, 1, depth_format, 0,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
}

#endif

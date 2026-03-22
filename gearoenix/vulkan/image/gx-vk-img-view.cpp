#include "gx-vk-img-view.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"

namespace {
[[nodiscard]] vk::ImageAspectFlags get_depth_stencil_aspect(const vk::Format format)
{
    switch (format) {
    case vk::Format::eD16Unorm:
    case vk::Format::eD32Sfloat:
    case vk::Format::eX8D24UnormPack32:
        return vk::ImageAspectFlagBits::eDepth;
    case vk::Format::eS8Uint:
        return vk::ImageAspectFlagBits::eStencil;
    case vk::Format::eD16UnormS8Uint:
    case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD32SfloatS8Uint:
        return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
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
    std::optional<std::uint32_t> layer_count,
    std::optional<vk::Format> format_override)
    : image(std::move(img))
    , extent(
          std::max(image->get_image_width() >> base_level, 1u),
          std::max(image->get_image_height() >> base_level, 1u),
          std::max(image->get_image_depth() >> base_level, 1u))
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

    vk::ImageViewCreateInfo info;
    info.image = image->get_vulkan_data();

    const bool is_cube = (image->get_flags() & vk::ImageCreateFlagBits::eCubeCompatible) && this->layer_count >= 6 && this->layer_count % 6 == 0;

    switch (image->get_image_type()) {
    case vk::ImageType::e1D:
        info.viewType = this->layer_count > 1 ? vk::ImageViewType::e1DArray : vk::ImageViewType::e1D;
        break;
    case vk::ImageType::e2D:
        if (is_cube) {
            info.viewType = this->layer_count > 6 ? vk::ImageViewType::eCubeArray : vk::ImageViewType::eCube;
        } else {
            info.viewType = this->layer_count > 1 ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D;
        }
        break;
    case vk::ImageType::e3D:
        info.viewType = vk::ImageViewType::e3D;
        break;
    default:
        GX_UNEXPECTED;
    }

    info.format = format_override.value_or(image->get_format());
    info.subresourceRange.baseMipLevel = this->base_level;
    info.subresourceRange.levelCount = this->level_count;
    info.subresourceRange.baseArrayLayer = this->base_layer;
    info.subresourceRange.layerCount = this->layer_count;

    if (image->get_usage() & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
        info.subresourceRange.aspectMask = get_depth_stencil_aspect(image->get_format());
    } else {
        info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }

    vulkan_data = vk::raii::ImageView(device::Logical::get().get_device(), info);
}

gearoenix::vulkan::image::View::~View() = default;

std::shared_ptr<gearoenix::vulkan::image::View> gearoenix::vulkan::image::View::create_depth_stencil(const std::string& name)
{
    const auto& physical_device = device::Physical::get();
    const auto depth_format = physical_device.get_supported_depth_format();
    const auto surf_cap = physical_device.get_surface_capabilities();
    return std::make_shared<View>(std::make_shared<Image>(
        name,
        surf_cap.currentExtent.width, surf_cap.currentExtent.height, 1,
        vk::ImageType::e2D, 1, 1, depth_format, vk::ImageCreateFlags { },
        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc));
}

#endif

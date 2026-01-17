#include "gx-vk-txt-2d.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-manager.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "gx-vk-txt-util.hpp"

gearoenix::vulkan::texture::Texture2D::Texture2D(const render::texture::TextureInfo& info, std::string && name)
    : render::texture::Texture2D(std::move(name), info)
    , view(new image::View(std::make_shared<image::Image>(
          info.get_width(), info.get_height(), info.get_depth(),
          convert_image_type(info.get_type()),
          static_cast<std::uint32_t>(compute_mipmaps_count(info)),
          1u,
          convert_image_format(info.get_format()),
          0u,
          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
          (render::texture::format_is_depth(info.get_format()) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT: VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT))))
    , mips([this] {
        std::vector<std::shared_ptr<image::View>> result;
        const auto mip_count = view->get_image()->get_mipmap_levels();
        result.reserve(mip_count);
        for (std::uint32_t mip = 0; mip < mip_count; ++mip) {
            result.push_back(std::make_shared<image::View>(std::shared_ptr(view->get_image()), mip, 1));
        }
        return result;
    }())
{}

gearoenix::vulkan::texture::Texture2D::~Texture2D() = default;

void gearoenix::vulkan::texture::Texture2D::write(
    const std::shared_ptr<platform::stream::Stream>&,
    const core::job::EndCaller<>&,
    bool) const
{
    GX_UNIMPLEMENTED;
}
#endif

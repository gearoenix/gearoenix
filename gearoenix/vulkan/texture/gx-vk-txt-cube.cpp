#include "gx-vk-txt-cube.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../image/gx-vk-img-image.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "gx-vk-txt-2d.hpp"
#include "gx-vk-txt-util.hpp"

gearoenix::vulkan::texture::TextureCube::TextureCube(const render::texture::TextureInfo& info, std::string&& name)
    : render::texture::TextureCube(std::move(name), info)
    , view(new image::View(std::make_shared<image::Image>(
        info.get_width(), info.get_height(), 1u,
        convert_image_type(info.get_type()),
        static_cast<std::uint32_t>(compute_mipmaps_count(info)),
        6u,
        convert_image_format(info.get_format()),
        VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
            (render::texture::format_is_depth(info.get_format()) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT: VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT))))
    , mips([this] {
        std::array<std::vector<std::shared_ptr<image::View>>, 6> result;
        const auto mip_count = view->get_image()->get_mipmap_levels();
        for (std::uint32_t face = 0; face < 6; ++face) {
            result[face].reserve(mip_count);
            for (std::uint32_t mip = 0; mip < mip_count; ++mip) {
                result[face].push_back(std::make_shared<image::View>(std::shared_ptr(view->get_image()), mip, 1, face, 1));
            }
        }
        return result;
    }())
{
}

gearoenix::vulkan::texture::TextureCube::~TextureCube() = default;

void gearoenix::vulkan::texture::TextureCube::write(
    const std::shared_ptr<platform::stream::Stream>&,
    const core::job::EndCaller<>&,
    const bool) const
{
    GX_UNIMPLEMENTED;
}

#endif

#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/texture/gx-rnd-txt-texture-2d.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::texture {
struct Texture2D final : render::texture::Texture2D {
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, view);

public:
    Texture2D(
        std::string name,
        const std::vector<std::vector<std::uint8_t>>& data,
        const render::texture::TextureInfo& info,
        std::uint64_t img_width,
        std::uint64_t img_height,
        const core::job::EndCaller<>& call);
    ~Texture2D() override;
    [[nodiscard]] static VkFormat convert(render::texture::TextureFormat format);

};
}
#endif
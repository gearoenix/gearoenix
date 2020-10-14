#ifndef GEAROENIX_VULKAN_TEXTURE_2D_HPP
#define GEAROENIX_VULKAN_TEXTURE_2D_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../render/texture/gx-rnd-txt-texture-2d.hpp"
#include "../../render/texture/gx-rnd-txt-texture-info.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::image {
class View;
}

namespace gearoenix::vulkan::texture {
class Texture2D final : public render::texture::Texture2D {
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, view)
public:
    Texture2D(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const std::vector<std::vector<std::uint8_t>>& data,
        const render::texture::TextureInfo& info,
        std::size_t img_width,
        std::size_t img_height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~Texture2D() noexcept final;
    void write_gx3d(
        const std::shared_ptr<system::stream::Stream>& s,
        const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>&) noexcept final;
    [[nodiscard]] static VkFormat convert(render::texture::TextureFormat format) noexcept;
};
}
#endif
#endif

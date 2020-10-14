#ifndef GEAROENIX_VULKAN_TEXTURE_CUBE_HPP
#define GEAROENIX_VULKAN_TEXTURE_CUBE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../render/texture/gx-rnd-txt-texture-cube.hpp"
#include "../../render/texture/gx-rnd-txt-texture-info.hpp"

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::image {
class View;
}

namespace gearoenix::vulkan::texture {
class TextureCube final : public render::texture::TextureCube {
    GX_GET_REFC_PRV(std::shared_ptr<image::View>, view)
public:
    TextureCube(
        core::Id id,
        std::string name,
        engine::Engine* engine,
        std::vector<std::vector<std::vector<std::uint8_t>>> data,
        const render::texture::TextureInfo& info,
        std::size_t aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~TextureCube() noexcept final;
    void write_gx3d(
        const std::shared_ptr<system::stream::Stream>& s,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept final;
};
}
#endif
#endif

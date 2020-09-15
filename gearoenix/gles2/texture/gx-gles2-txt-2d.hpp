#ifndef GEAROENIX_GLES2_TEXTURE_2D_HPP
#define GEAROENIX_GLES2_TEXTURE_2D_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../gl/gx-gl-types.hpp"
#include "../../render/texture/gx-rnd-txt-texture-2d.hpp"
#include "../../render/texture/gx-rnd-txt-texture-info.hpp"

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::texture {
class Texture2D final : public render::texture::Texture2D {
private:
    GX_GET_VAL_PRV(gl::uint, texture_object, 0)

    Texture2D(
        core::Id id,
        std::string name,
        render::texture::TextureFormat texture_format,
        const render::texture::SampleInfo& sample_info,
        engine::Engine* e) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Texture2D> construct(
        core::Id id,
        std::string name,
        engine::Engine* e,
        std::vector<std::vector<std::uint8_t>> data,
        const render::texture::TextureInfo& info,
        std::size_t img_width,
        std::size_t img_height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~Texture2D() noexcept final;
    void write_gx3d(
        const std::shared_ptr<system::stream::Stream>& s,
        const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>&) noexcept final;
    void bind(gl::enumerated texture_unit) const noexcept;
    void bind() const noexcept;
    [[nodiscard]] static gl::uint convert(render::texture::TextureFormat format) noexcept;
};
}
#endif
#endif

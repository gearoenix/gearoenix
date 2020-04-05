#ifndef GEAROENIX_GLES2_TEXTURE_CUBE_HPP
#define GEAROENIX_GLES2_TEXTURE_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-texture-cube.hpp"
#include "../../render/texture/rnd-txt-texture-info.hpp"

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::texture {
class TextureCube final : public render::texture::TextureCube {
    GX_GET_VAL_PRV(gl::uint, texture_object, 0)
private:
    TextureCube(core::Id id, render::texture::TextureFormat texture_format, engine::Engine* e) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<TextureCube> construct(
        core::Id id,
        engine::Engine* e,
        const void* data,
        const render::texture::TextureInfo& info,
        unsigned int aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    [[nodiscard]] static std::shared_ptr<TextureCube> construct(
        core::Id id,
        engine::Engine* e,
        const render::texture::TextureInfo& info,
        unsigned int aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~TextureCube() noexcept final;
    void write_gx3d(
        const std::shared_ptr<system::stream::Stream>& s,
        const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>&) noexcept final;
    void bind(gl::enumerated texture_unit) const noexcept;
    void bind() const noexcept;
};
}

#endif
#endif

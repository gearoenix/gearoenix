#ifndef GEAROENIX_GLES2_TEXTURE_2D_HPP
#define GEAROENIX_GLES2_TEXTURE_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-texture-2d.hpp"
#include "../../render/texture/rnd-txt-texture-info.hpp"

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::texture {
class Texture2D final : public render::texture::Texture2D {
private:
    GX_GET_VAL_PRV(gl::uint, texture_object, 0)

    Texture2D(core::Id id, render::texture::TextureFormat texture_format, engine::Engine* e) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Texture2D> construct(
        core::Id id,
        engine::Engine* e,
        const void* data,
        const render::texture::TextureInfo& info,
        unsigned int img_width,
        unsigned int img_height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    [[nodiscard]] static std::shared_ptr<Texture2D> construct(
        core::Id id,
        engine::Engine* e,
        const render::texture::TextureInfo& info,
        unsigned int width,
        unsigned int height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    //    Texture2D(core::Id my_id, gl::uint txt_obj, engine::Engine* e) noexcept;
    ~Texture2D() noexcept final;
    void bind(gl::enumerated texture_unit) const noexcept;
    void bind() const noexcept;
    [[nodiscard]] static gl::uint convert(render::texture::TextureFormat format) noexcept;
};
}
#endif
#endif

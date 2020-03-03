#ifndef GEAROENIX_GLC3_TEXTURE_2D_HPP
#define GEAROENIX_GLC3_TEXTURE_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-texture-2d.hpp"
#include "../../render/texture/rnd-txt-texture-info.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::texture {
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
        unsigned int img_width,
        unsigned int img_height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    Texture2D(core::Id my_id, gl::uint txt_obj, render::texture::TextureFormat texture_format, engine::Engine* e) noexcept;
    ~Texture2D() noexcept final;
    void bind(gl::enumerated texture_unit) const noexcept;
    void bind() const noexcept;
    void generate_mipmap() const noexcept;
    [[nodiscard]] static gl::sint convert_internal_format(render::texture::TextureFormat f) noexcept;
    [[nodiscard]] static gl::enumerated convert_format(render::texture::TextureFormat f) noexcept;
    [[nodiscard]] static gl::enumerated convert_data_format(render::texture::TextureFormat f) noexcept;
};
}

#endif
#endif

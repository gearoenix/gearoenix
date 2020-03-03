#ifndef GEAROENIX_GLC3_TEXTURE_CUBE_HPP
#define GEAROENIX_GLC3_TEXTURE_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-texture-cube.hpp"
#include "../../render/texture/rnd-txt-texture-info.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::texture {
class TextureCube final : public render::texture::TextureCube {
    GX_GET_VAL_PRV(gl::uint, texture_object, 0)
private:
    TextureCube(core::Id id, render::texture::TextureFormat texture_format, engine::Engine* engine) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<TextureCube> construct(
        core::Id id,
        engine::Engine* engine,
        const void* data,
        const render::texture::TextureInfo& info,
        unsigned int aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    [[nodiscard]] static std::shared_ptr<TextureCube> construct(
        core::Id id,
        engine::Engine* engine,
        const render::texture::TextureInfo& info,
        unsigned int aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~TextureCube() noexcept final;
    void bind(gl::enumerated texture_unit) const noexcept;
    void bind() const noexcept;
    void generate_mipmap() const noexcept;
};
}

#endif
#endif

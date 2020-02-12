#ifndef GEAROENIX_GLES2_TEXTURE_TARGET_2D_HPP
#define GEAROENIX_GLES2_TEXTURE_TARGET_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/texture/rnd-txt-target-2d.hpp"
#include "gles2-txt-target.hpp"

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::texture {
class Texture2D;
class Target2D : public render::texture::Target2D {
private:
    const std::shared_ptr<gles2::texture::Target> base;
    const std::shared_ptr<Texture2D> txt;

    Target2D(
        core::Id id,
        engine::Engine* e,
        const render::texture::Info& info,
        unsigned int width,
        unsigned int height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    Target2D(const Target2D& o) noexcept;

public:
    explicit Target2D(engine::Engine* e) noexcept;
    ~Target2D() noexcept final;
    [[nodiscard]] static std::shared_ptr<Target2D> construct(
        core::Id id,
        engine::Engine* e,
        const render::texture::Info& info,
        unsigned int width,
        unsigned int height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void bind() const noexcept;
    void bind_texture(gl::enumerated texture_unit) const noexcept;
    [[nodiscard]] const render::texture::Texture2D* get_texture_2d(std::size_t index) const noexcept final;
    [[nodiscard]] std::size_t get_textures_count() const noexcept final;
    [[nodiscard]] render::texture::Target* clone() const noexcept final;
};
}

#endif
#endif

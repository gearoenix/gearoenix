#ifndef GEAROENIX_GLC3_TEXTURE_TARGET_2D_HPP
#define GEAROENIX_GLC3_TEXTURE_TARGET_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-info.hpp"
#include "../../render/texture/rnd-txt-target-2d.hpp"
#include <vector>

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::texture {
class Target;
class Texture2D;
class Target2D : public render::texture::Target2D {
private:
    const std::shared_ptr<glc3::texture::Target> base;
    const std::vector<std::shared_ptr<Texture2D>> textures;

public:
    void bind() const noexcept;
    void bind_textures(const gl::enumerated* texture_units, std::size_t count) const noexcept;
    [[nodiscard]] std::shared_ptr<render::texture::Texture2D> get_texture_2d(std::size_t index) const noexcept final;
    [[nodiscard]] std::size_t get_textures_count() const noexcept final;
    [[nodiscard]] render::texture::Target* clone() const noexcept final;
};
}

#endif
#endif

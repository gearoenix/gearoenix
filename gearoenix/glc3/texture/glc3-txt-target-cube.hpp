#ifndef GEAROENIX_GLC3_TEXTURE_TARGET_CUBE_HPP
#define GEAROENIX_GLC3_TEXTURE_TARGET_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-face.hpp"
#include "../../render/texture/rnd-txt-target-cube.hpp"
#include <vector>

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::texture {
class Target;
class TextureCube;
class TargetCube : public render::texture::TargetCube {
private:
    const std::shared_ptr<glc3::texture::Target> base;
    const std::vector<std::pair<render::texture::Face, std::shared_ptr<TextureCube>>> face_textures;

    TargetCube(
        core::Id id,
        engine::Engine* e,
        std::vector<std::pair<render::texture::Face, std::shared_ptr<TextureCube>>> face_textures) noexcept;

public:
    ~TargetCube() noexcept final;
    [[nodiscard]] static std::shared_ptr<TargetCube> construct(
        core::Id my_id,
        engine::Engine* e,
        std::vector<std::pair<render::texture::Face, std::shared_ptr<TextureCube>>> face_textures,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void bind() const noexcept;
    void bind_textures(const std::vector<gl::enumerated>& texture_units) const noexcept;
    [[nodiscard]] std::shared_ptr<render::texture::TextureCube> get_texture_cube(std::size_t index) const noexcept final;
    [[nodiscard]] std::size_t get_textures_count() const noexcept final;
    [[nodiscard]] render::texture::Target* clone() const noexcept final;
};
}

#endif
#endif

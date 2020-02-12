#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_CUBE_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_CUBE_HPP
#include "rnd-txt-target.hpp"
#include "rnd-txt-texture-cube.hpp"

namespace gearoenix::render::texture {
class TargetCube : public Target {
protected:
    TargetCube(const core::Id texture_id, engine::Engine* const e) noexcept
        : Target(Type::TargetCube, texture_id, e)
    {
    }

public:
    ~TargetCube() noexcept override = default;

    [[nodiscard]] std::shared_ptr<Texture> get_texture(const std::size_t index) const noexcept override
    {
        return get_texture_cube(index);
    }

    [[nodiscard]] virtual std::shared_ptr<TextureCube> get_texture_cube(std::size_t index) const noexcept = 0;
};
}
#endif

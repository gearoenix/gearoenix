#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_CUBE_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_CUBE_HPP
#include "rnd-txt-target-2d.hpp"
#include "rnd-txt-texture-cube.hpp"

namespace gearoenix::render::texture {
class TargetCube : public TextureCube, public Target {
protected:
    TargetCube(const core::Id my_id, engine::Engine* const e) noexcept
        : Texture(my_id, Type::TargetCube, e)
        , TextureCube(my_id, e)
        , Target(Type::TargetCube, my_id, e)
    {
    }

public:
    ~TargetCube() noexcept override = default;
};
}
#endif

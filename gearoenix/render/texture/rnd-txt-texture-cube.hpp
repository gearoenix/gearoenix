#ifndef GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#define GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix::render::texture {
class Cube : public Texture {
protected:
    Cube(const core::Id my_id, engine::Engine* const engine, const Type::Id texture_type = Type::CUBE) noexcept
        : Texture(my_id, texture_type, engine)
    {
    }

public:
    virtual ~Cube() noexcept = default;
};
}
#endif

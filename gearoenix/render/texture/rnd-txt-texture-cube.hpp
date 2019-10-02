#ifndef GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#define GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix::render::texture {
class Cube : public Texture {
protected:
    Cube(const core::Id my_id, engine::Engine* const engine, const Type texture_type = Type::TextureCube) noexcept
        : Texture(my_id, texture_type, engine)
    {
    }

public:
    virtual ~Cube() noexcept = default;
};
}
#endif

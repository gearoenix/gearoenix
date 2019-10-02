#ifndef GEAROENIX_RENDER_TEXTURE_TEXTURE_2D_HPP
#define GEAROENIX_RENDER_TEXTURE_TEXTURE_2D_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix::render::texture {
class Texture2D : public Texture {
protected:
    unsigned int img_width = 0;
    unsigned int img_height = 0;
    Texture2D(const core::Id my_id, engine::Engine* const e, const Type texture_type = Type::Texture2D) noexcept
        : Texture(my_id, texture_type, e)
    {
    }

public:
    virtual ~Texture2D() noexcept = default;
};
}
#endif

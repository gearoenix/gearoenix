#ifndef GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#define GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix::render::texture {
class TextureCube : virtual public Texture {
    GX_GET_VAL_PRT(std::size_t, img_width, 0)
    GX_GET_VAL_PRT(std::size_t, img_height, 0)
protected:
    TextureCube(const core::Id my_id, engine::Engine* const engine) noexcept
        : Texture(my_id, Type::TextureCube, engine)
    {
    }

public:
    ~TextureCube() noexcept override = default;
};
}
#endif

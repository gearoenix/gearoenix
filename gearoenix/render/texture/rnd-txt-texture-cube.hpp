#ifndef GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#define GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix::render::texture {
class TextureCube : public Texture {
    GX_GET_VAL_PRT(std::size_t, aspect, 0)
protected:
    TextureCube(
        const core::Id id,
        const TextureFormat texture_format,
        engine::Engine* const engine) noexcept
        : Texture(id, Type::TextureCube, texture_format, engine)
    {
    }

public:
    ~TextureCube() noexcept override = default;
};
}
#endif

#ifndef GEAROENIX_RENDER_TEXTURE_TEXTURE_2D_HPP
#define GEAROENIX_RENDER_TEXTURE_TEXTURE_2D_HPP
#include "gx-rnd-txt-texture.hpp"

namespace gearoenix::render::texture {
struct Texture2D : Texture {
protected:
    Texture2D(
        std::string name,
        const TextureInfo& info,
        engine::Engine& e);

public:
    ~Texture2D() override;
};
}
#endif

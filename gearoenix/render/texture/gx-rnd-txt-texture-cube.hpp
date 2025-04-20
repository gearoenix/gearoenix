#pragma once
#include "gx-rnd-txt-texture.hpp"

namespace gearoenix::render::texture {
struct TextureCube : Texture {
protected:
    TextureCube(std::string name, const TextureInfo& info);

public:
    ~TextureCube() override;
};
}
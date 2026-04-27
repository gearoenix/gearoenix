#pragma once
#include "gx-rnd-txt-texture.hpp"

namespace gearoenix::render::texture {
struct Texture3D : Texture {
protected:
    Texture3D(std::string&& name, const TextureInfo& info);

public:
    ~Texture3D() override;
};
}

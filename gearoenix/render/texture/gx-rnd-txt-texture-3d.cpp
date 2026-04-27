#include "gx-rnd-txt-texture-3d.hpp"

gearoenix::render::texture::Texture3D::Texture3D(std::string&& name, const TextureInfo& info)
    : Texture(std::move(name), info)
{
}

gearoenix::render::texture::Texture3D::~Texture3D() = default;

#include "gx-rnd-txt-texture-cube.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"

gearoenix::render::texture::TextureCube::TextureCube(std::string name, const TextureInfo& info)
    : Texture(std::move(name), info)
{
}

gearoenix::render::texture::TextureCube::~TextureCube() = default;
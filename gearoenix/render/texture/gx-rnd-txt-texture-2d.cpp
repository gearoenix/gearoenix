#include "gx-rnd-txt-texture-2d.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"

gearoenix::render::texture::Texture2D::Texture2D(
    std::string name,
    const TextureInfo& info,
    engine::Engine& e) noexcept
    : Texture(std::move(name), info, e)
{
}

gearoenix::render::texture::Texture2D::~Texture2D() noexcept = default;
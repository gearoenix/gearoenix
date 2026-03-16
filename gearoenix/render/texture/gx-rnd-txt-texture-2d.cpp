#include "gx-rnd-txt-texture-2d.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"

gearoenix::render::texture::Texture2D::Texture2D(std::string name, const TextureInfo& info)
    : Texture(std::move(name), info)
{
}

gearoenix::render::texture::Texture2D::~Texture2D() = default;

void* gearoenix::render::texture::Texture2D::get_imgui_ptr() const { return nullptr; }

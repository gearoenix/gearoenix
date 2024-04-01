#ifndef GEAROENIX_RENDER_SKYBOX_TYPES_HPP
#define GEAROENIX_RENDER_SKYBOX_TYPES_HPP
#include <memory>
#include <variant>

namespace gearoenix::render::texture {
struct Texture2D;
struct TextureCube;
}

namespace gearoenix::render::skybox {
typedef std::variant<std::shared_ptr<texture::Texture2D>, std::shared_ptr<texture::TextureCube>> Texture;
}

#endif
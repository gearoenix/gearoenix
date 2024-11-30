#pragma once
#include <cstdint>
#include <string>

namespace gearoenix::render::texture {
enum struct Type : std::uint8_t {
    Texture2D = 1,
    Texture3D = 2,
    TextureCube = 3,
    Target = 4,
    Unknown = 255,
};
}

namespace std {
[[nodiscard]] std::string to_string(gearoenix::render::texture::Type t);
}
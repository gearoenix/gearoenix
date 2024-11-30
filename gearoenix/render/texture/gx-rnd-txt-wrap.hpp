#pragma once
#include <cstdint>
#include <string>

namespace gearoenix::render::texture {
enum struct Wrap : std::uint8_t {
    ClampToEdge = 1,
    Mirror = 2,
    Repeat = 3,
};
}

namespace std {
[[nodiscard]] std::string to_string(gearoenix::render::texture::Wrap w);
}
#pragma once
#include <cstdint>
#include <string>

namespace gearoenix::render::texture {
enum struct Face : std::uint8_t {
    PositiveX = 0,
    NegativeX = 1,
    PositiveY = 2,
    NegativeY = 3,
    PositiveZ = 4,
    NegativeZ = 5,
};

constexpr Face FACES[6] = {
    Face::PositiveX,
    Face::NegativeX,
    Face::PositiveY,
    Face::NegativeY,
    Face::PositiveZ,
    Face::NegativeZ,
};
}

namespace std {
[[nodiscard]] std::string to_string(gearoenix::render::texture::Face f);
}
#pragma once
#include <cstdint>
#include <string>

namespace gearoenix::render::texture {
enum struct Filter : std::uint8_t {
    Cubic = 1,
    CubicMipmapCubic = 2,
    CubicMipmapLinear = 3,
    CubicMipmapNearest = 4,
    Linear = 5,
    LinearMipmapCubic = 6,
    LinearMipmapLinear = 7,
    LinearMipmapNearest = 8,
    Nearest = 9,
    NearestMipmapCubic = 10,
    NearestMipmapLinear = 11,
    NearestMipmapNearest = 12,
};
}

namespace std {
[[nodiscard]] std::string to_string(gearoenix::render::texture::Filter f);
}

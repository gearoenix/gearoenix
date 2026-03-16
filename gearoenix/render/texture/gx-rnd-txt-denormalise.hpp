#pragma once
#include <cstdint>

namespace gearoenix::render::texture {
[[nodiscard]] constexpr std::uint8_t denormalise(const float v)
{
    if (v >= 0.999f) {
        return 255;
    }
    if (v <= 0.001f) {
        return 0;
    }
    return static_cast<std::uint8_t>(v * 255.0f + 0.49f);
}
}
#pragma once
#include <cstdint>

namespace gearoenix::render::widget {
enum struct Alignment : std::uint8_t {
    Center = 1,
    Start = 2,
    End = 3,
};
}
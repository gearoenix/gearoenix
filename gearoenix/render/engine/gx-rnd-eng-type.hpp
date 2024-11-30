#pragma once
#include <cstdint>
#include <ostream>

namespace gearoenix::render::engine {
enum struct Type : std::uint8_t {
    Vulkan = 1,
    OpenGL = 2,
    Direct3D = 3,
    Metal = 4,
};

[[nodiscard]] const char* to_string(Type);

std::ostream& operator<<(std::ostream& os, Type t);
}
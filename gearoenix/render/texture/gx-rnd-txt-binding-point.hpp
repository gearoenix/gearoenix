#pragma once
#include <cstdint>

namespace gearoenix::render::texture {
enum struct BindingPoint : std::uint8_t {
    Unknown = 0,
    Albedo = 1,
    Normal = 2,
    Emission = 3,
    Orm = 4,
};
}
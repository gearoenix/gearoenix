#ifndef GEAROENIX_RENDER_ENGINE_TYPE_HPP
#define GEAROENIX_RENDER_ENGINE_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
#include <ostream>

namespace gearoenix::render::engine {
enum struct Type : core::Id {
    Vulkan = 1,
    OpenGL = 2,
    Direct3D = 3,
    Metal = 4,
};

[[nodiscard]] const char* to_string(Type) noexcept;

std::ostream& operator<<(std::ostream& os, Type t) noexcept;
}
#endif

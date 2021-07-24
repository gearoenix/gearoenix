#ifndef GEAROENIX_RENDER_ENGINE_TYPE_HPP
#define GEAROENIX_RENDER_ENGINE_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
#include <ostream>

namespace gearoenix::render::engine {
enum struct Type : core::Id {
    Vulkan = 1,
    OpenGL = 2,
    DirectX11 = 3,
    DirectX12 = 4,
    Metal = 5,
};
std::ostream& operator<<(std::ostream& os, Type t) noexcept;
}
#endif

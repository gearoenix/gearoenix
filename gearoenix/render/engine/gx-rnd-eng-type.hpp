#ifndef GEAROENIX_RENDER_ENGINE_TYPE_HPP
#define GEAROENIX_RENDER_ENGINE_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
#include <ostream>

namespace gearoenix::render::engine {
enum struct Type : core::Id {
    None = 0,
    Vulkan = 1,
    OpenGL43 = 2,
    OpenGL33 = 3,
    OpenGLES3 = 4,
    OpenGLES2 = 5,
    DirectX11 = 6,
    DirectX12 = 7,
    Metal = 8,
};
std::ostream& operator<<(std::ostream& os, Type t) noexcept;
}
#endif

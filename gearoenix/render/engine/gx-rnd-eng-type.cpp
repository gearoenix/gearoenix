#include "gx-rnd-eng-type.hpp"

namespace gearoenix::render::engine {
std::ostream& operator<<(std::ostream& os, const Type t) noexcept
{
    switch (t) {
    case Type::Vulkan:
        os << "Vulkan";
        break;
    case Type::OpenGL:
        os << "OpenGL";
        break;
    case Type::Direct3DX:
        os << "Direct3DX";
        break;
    case Type::Metal:
        os << "Metal";
        break;
    }
    return os;
}
}
#include "gx-rnd-eng-type.hpp"

namespace gearoenix::render::engine {
const char* to_string(const Type t)
{
    switch (t) {
    case Type::Vulkan:
        return "Vulkan";
    case Type::OpenGL:
        return "OpenGL";
    case Type::Direct3D:
        return "Direct3D";
    case Type::Metal:
        return "Metal";
    }
    return nullptr;
}

std::ostream& operator<<(std::ostream& os, const Type t)
{
    os << to_string(t);
    return os;
}
}
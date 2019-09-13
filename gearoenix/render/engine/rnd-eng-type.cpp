#include "rnd-eng-type.hpp"

namespace gearoenix::render::engine {
std::ostream& operator<<(std::ostream& os, const Type t) noexcept
{
    switch (t) {
    case Type::NONE:
        os << "NONE";
        break;
    case Type::VULKAN:
        os << "VULKAN";
        break;
    case Type::OPENGL_43:
        os << "OPENGL_43";
        break;
    case Type::OPENGL_33:
        os << "OPENGL_33";
        break;
    case Type::OPENGL_ES3:
        os << "OPENGL_ES3";
        break;
    case Type::OPENGL_ES2:
        os << "OPENGL_ES2";
        break;
    case Type::DIRECTX_11:
        os << "DIRECTX_11";
        break;
    case Type::DIRECTX_12:
        os << "DIRECTX_12";
        break;
    case Type::METAL:
        os << "METAL";
        break;
    }
    return os;
}
}
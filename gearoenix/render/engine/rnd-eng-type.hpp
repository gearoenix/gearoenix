#ifndef GEAROENIX_RENDER_ENGINE_TYPE_HPP
#define GEAROENIX_RENDER_ENGINE_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::engine {
enum struct Type : core::Id {
    NONE = 0,
    VULKAN = 1,
    OPENGL_43 = 2,
    OPENGL_33 = 3,
    OPENGL_ES3 = 4,
    OPENGL_ES2 = 5,
    DIRECTX_11 = 6,
    DIRECTX_12 = 7,
    METAL = 8,
};
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
#endif

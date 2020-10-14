#include "gx-rnd-eng-type.hpp"

namespace gearoenix::render::engine {
std::ostream& operator<<(std::ostream& os, const Type t) noexcept
{
    switch (t) {
    case Type::None:
        os << "None";
        break;
    case Type::Vulkan:
        os << "Vulkan";
        break;
    case Type::OpenGL43:
        os << "OpenGL43";
        break;
    case Type::OpenGL33:
        os << "OpenGL33";
        break;
    case Type::OpenGLES3:
        os << "OpenGLES3";
        break;
    case Type::OpenGLES2:
        os << "OpenGLES2";
        break;
    case Type::DirectX11:
        os << "DirectX11";
        break;
    case Type::DirectX12:
        os << "DirectX12";
        break;
    case Type::Metal:
        os << "Metal";
        break;
    }
    return os;
}
}
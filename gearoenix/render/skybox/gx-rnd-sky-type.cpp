#include "gx-rnd-sky-type.hpp"
#include "../../platform/gx-plt-log.hpp"

std::ostream& operator<<(std::ostream& os, gearoenix::render::skybox::Type t) noexcept
{
    switch (t) {
    case gearoenix::render::skybox::Type::Cube:
        os << "\"Cube\"";
        break;
    case gearoenix::render::skybox::Type::Equirectangular:
        os << "\"Equirectangular\"";
        break;
    default:
        GXLOGF("Unexpected Skybox type " << static_cast<gearoenix::core::TypeId>(t))
    }
    return os;
}
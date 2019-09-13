#include "rnd-scn-type.hpp"
#include "../../system/sys-log.hpp"

std::ostream& operator<<(std::ostream& os, gearoenix::render::scene::Type t) noexcept
{
    switch (t) {
    case gearoenix::render::scene::Type::Game:
        os << "\"Game\"";
        break;
    case gearoenix::render::scene::Type::Ui:
        os << "\"Ui\"";
        break;
    default:
        GXLOGF("Unexpected scene type " << static_cast<gearoenix::core::TypeId>(t))
        break;
    }
    return os;
}

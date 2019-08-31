#include "phs-cld-type.hpp"
#include "../../system/sys-log.hpp"

std::ostream& gearoenix::physics::collider::operator<<(std::ostream& os, const Type t) noexcept 
{
    os << "physics::collider::Type<";
    switch (t)
    {
    case Type::GHOST:
        os << "GHOST";
        break;
    case Type::MESH:
        os << "MESH";
        break;
    case Type::AABB:
        os << "AABB";
        break;
    case Type::SPHERE:
        os << "SPHERE";
        break;
    default:
        GXLOGF("Unexpected value for physics::collider::Type.")
    }
    os << ">";
    return os;
}
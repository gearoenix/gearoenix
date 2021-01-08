#include "gx-phs-cld-type.hpp"
#include "../../platform/gx-plt-log.hpp"

std::ostream& gearoenix::physics::collider::operator<<(std::ostream& os, const Type t) noexcept
{
    os << "physics::collider::Type<";
    switch (t) {
    case Type::Ghost:
        os << "Ghost";
        break;
    case Type::Mesh:
        os << "Mesh";
        break;
    case Type::Aabb:
        os << "Aabb";
        break;
    case Type::Sphere:
        os << "Sphere";
        break;
    case Type::Frustum:
        os << "Frustum";
        break;
    case Type::Transform:
        os << "Transform";
        break;
    default:
        GXLOGF("Unexpected value for physics::collider::Type.")
    }
    os << ">";
    return os;
}
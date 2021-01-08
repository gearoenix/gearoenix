#include "gx-math-intersection-status.hpp"
#include "../platform/gx-plt-log.hpp"

std::ostream& operator<<(std::ostream& os, const gearoenix::math::IntersectionStatus i) noexcept
{
    switch (i) {
    case gearoenix::math::IntersectionStatus::Above:
        os << "'Above'";
        break;
    case gearoenix::math::IntersectionStatus::Cut:
        os << "'Cut'";
        break;
    case gearoenix::math::IntersectionStatus::In:
        os << "'In'";
        break;
    case gearoenix::math::IntersectionStatus::Out:
        os << "'Out'";
        break;
    case gearoenix::math::IntersectionStatus::Tangent:
        os << "'Tangent'";
        break;
    case gearoenix::math::IntersectionStatus::Under:
        os << "'Under'";
        break;
    default:
        GX_UNEXPECTED
        break;
    }
    return os;
}
#ifndef GEAROENIX_MATH_INTERSECTION_STATUS_HPP
#define GEAROENIX_MATH_INTERSECTION_STATUS_HPP
#include "../platform/gx-plt-log.hpp"
#include <ostream>

#ifdef Above
#undef Above
#endif

namespace gearoenix::math {
enum struct IntersectionStatus {
    Above = 1,
    Cut = 2,
    In = 3,
    Out = 4,
    Touch = 5,
    Under = 6,
};
}

inline std::ostream& operator<<(std::ostream& os, const gearoenix::math::IntersectionStatus i) noexcept
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
    case gearoenix::math::IntersectionStatus::Touch:
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
#endif
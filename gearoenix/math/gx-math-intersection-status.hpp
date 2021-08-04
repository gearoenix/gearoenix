#ifndef GEAROENIX_MATH_INTERSECTION_STATUS_HPP
#define GEAROENIX_MATH_INTERSECTION_STATUS_HPP
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
    Tangent = 5,
    Under = 6,
};
}
std::ostream& operator<<(std::ostream&, gearoenix::math::IntersectionStatus) noexcept;
#endif
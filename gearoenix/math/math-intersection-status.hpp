#ifndef GEAROENIX_MATH_INTERSECTION_SATUTS_HPP
#define GEAROENIX_MATH_INTERSECTION_SATUTS_HPP
namespace gearoenix::math {
enum struct IntersectionStatus {
    ABOVE = 1,
    CUT = 2,
    IN = 3,
    OUT = 4,
    TANGENT = 5,
    UNDER = 6,
};
}
#endif
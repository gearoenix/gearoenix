#ifndef GEAROENIX_MATH_PLANE_HPP
#define GEAROENIX_MATH_PLANE_HPP
#include "math-intersection-status.hpp"
#include "math-vector-3d.hpp"

namespace gearoenix::math {
struct Sphere;
struct Plane {
    Vec3<double> normal;
    double d;
    Vec3<double> point;

    Plane() = delete;
    explicit Plane(const Vec3<double>& point, const Vec3<double>& normal) noexcept;

    [[nodiscard]] IntersectionStatus check_intersection(const Sphere& s) const noexcept;
};
}
#endif
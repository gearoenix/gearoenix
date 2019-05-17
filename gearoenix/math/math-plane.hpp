#ifndef GEAROENIX_MATH_PLANE_HPP
#define GEAROENIX_MATH_PLANE_HPP
#include "math-intersection-status.hpp"
#include "math-vector.hpp"
namespace gearoenix {
namespace math {
    struct Sphere;
    struct Plane {
        Vec3 normal;
        core::Real d;
        Vec3 point;

        Plane() = delete;
        explicit Plane(const Vec3& point, const Vec3& normal) noexcept;

        IntersectionStatus::Type check_intersection(const Sphere& s) const noexcept;
    };
}
}
#endif
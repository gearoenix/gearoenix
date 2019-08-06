#ifndef GEAROENIX_MATH_SPHERE_HPP
#define GEAROENIX_MATH_SPHERE_HPP
#include "math-intersection-status.hpp"
#include "math-vector.hpp"
namespace gearoenix::math {
struct Sphere {
    Vec3 position;
    core::Real radius;

    Sphere() = delete;
    Sphere(const Vec3& position, core::Real radius) noexcept;
    void insert(core::Real r) noexcept;

    IntersectionStatus::Type check_intersection(const Sphere& o) const noexcept;
};
}
#endif
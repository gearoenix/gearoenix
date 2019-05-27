#ifndef GEAROENIX_MATH_AABB_HPP
#define GEAROENIX_MATH_AABB_HPP
#include "math-intersection-status.hpp"
#include "math-vector.hpp"
namespace gearoenix::math {
struct Ray3;
struct Sphere;
struct Aabb3 {
    Vec3 mx;
    Vec3 mn;
    explicit Aabb3() noexcept;
    explicit Aabb3(const Vec3& mx, const Vec3& mn) noexcept;
    void reset() noexcept;
    void reset(const Vec3& p) noexcept;
    void put(const Vec3& p) noexcept;
    void put(const Sphere& o) noexcept;
    bool test(const Ray3& r, core::Real& t_min_result) const noexcept;
    bool test(const Aabb3& o, Aabb3& intersection) const noexcept;
    bool test(const Aabb3& o) const noexcept;
    bool test(const Sphere& o) const noexcept;
    IntersectionStatus::Type check_intersection(const Aabb3& o) const noexcept;
};
}
#endif

#ifndef GEAROENIX_MATH_SPHERE_HPP
#define GEAROENIX_MATH_SPHERE_HPP
#include "../core/cr-static.hpp"
#include "math-intersection-status.hpp"
#include "math-vector.hpp"
#include <optional>

namespace gearoenix::math {
struct Ray3;
struct Sphere {
    GX_GET_REF_PRV(Vec3, position)
    GX_GET_VAL_PRV(core::Real, radius, 0.0F)
private:
    core::Real radius2 = 0.0F;
public:
    Sphere() = delete;
    Sphere(const Vec3& position, core::Real radius) noexcept;
    void set_radius(core::Real r) noexcept;
    void insert(core::Real r) noexcept;
    std::optional<core::Real> hit(const math::Ray3& r) const noexcept;
    std::optional<core::Real> hit(const math::Ray3& r, core::Real d_min) const noexcept;
    IntersectionStatus::Type check_intersection(const Sphere& o) const noexcept;
};
}
#endif
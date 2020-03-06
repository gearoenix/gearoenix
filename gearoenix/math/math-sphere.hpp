#ifndef GEAROENIX_MATH_SPHERE_HPP
#define GEAROENIX_MATH_SPHERE_HPP
#include "../core/cr-static.hpp"
#include "math-intersection-status.hpp"
#include "math-vector-3d.hpp"
#include <optional>

namespace gearoenix::math {
struct Ray3;
struct Sphere {
    GX_GET_CREF_PRV(Vec3<double>, center)
    GX_GET_VAL_PRV(core::Real, radius, 0.0F)
private:
    core::Real radius2 = 0.0F;

public:
    Sphere() = delete;
    Sphere(const Vec3<double>& center, core::Real radius) noexcept;
    void set_radius(core::Real r) noexcept;
    void set_center(const Vec3<double>& center) noexcept;
    void insert(core::Real r) noexcept;
    std::optional<core::Real> hit(const math::Ray3& r) const noexcept;
    std::optional<core::Real> hit(const math::Ray3& r, core::Real d_min) const noexcept;
    IntersectionStatus check_intersection(const Sphere& o) const noexcept;
};
}
#endif
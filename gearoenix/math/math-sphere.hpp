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
    GX_GET_VAL_PRV(double, radius, 0.0)
    GX_GET_VAL_PRV(double, radius2, 0.0)

public:
    Sphere() = delete;
    Sphere(const Vec3<double>& center, double radius) noexcept;
    void set_radius(double r) noexcept;
    void set_center(const Vec3<double>& center) noexcept;
    void insert(double r) noexcept;
    [[nodiscard]] std::optional<double> hit(const math::Ray3& r) const noexcept;
    [[nodiscard]] std::optional<double> hit(const math::Ray3& r, double d_min) const noexcept;
    [[nodiscard]] IntersectionStatus check_intersection(const Sphere& o) const noexcept;
};
}
#endif
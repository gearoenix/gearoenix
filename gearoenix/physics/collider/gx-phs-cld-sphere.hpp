#ifndef GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#define GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#include "../../math/gx-math-sphere.hpp"

namespace gearoenix::physics::collider {
struct Sphere final {
    GX_GET_CREF_PRV(math::Sphere<double>, sphere)

public:
    Sphere(const math::Vec3<double>& c, const double r) noexcept
        : sphere(c, r)
    {
    }
    void set_location(const math::Vec3<double>& l) noexcept { sphere.set_center(l); }
    void local_scale(const double s) noexcept { sphere.set_radius(sphere.get_radius() * s); }
    [[nodiscard]] std::optional<double> hit(const math::Ray3<double>& r, const double d_min) const noexcept { return sphere.hit(r, d_min); }
};
}
#endif

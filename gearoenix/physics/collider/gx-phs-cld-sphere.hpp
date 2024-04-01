#ifndef GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#define GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#include "../../math/gx-math-sphere.hpp"

namespace gearoenix::physics::collider {
struct Sphere final {
    GX_GET_CREF_PRV(math::Sphere<double>, sphere)

public:
    Sphere(const math::Vec3<double>& c, const double r)
        : sphere(c, r)
    {
    }
    void set_location(const math::Vec3<double>& l) { sphere.set_center(l); }
    void local_scale(const double s) { sphere.set_radius(sphere.get_radius() * s); }
    [[nodiscard]] std::optional<double> hit(const math::Ray3<double>& r, const double d_min) const { return sphere.hit(r, d_min); }
};
}
#endif

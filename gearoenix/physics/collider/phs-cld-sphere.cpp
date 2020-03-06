#include "phs-cld-sphere.hpp"
#include "../../math/math-ray.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"

gearoenix::physics::collider::Sphere::Sphere(const math::Vec3<double>& c, core::Real r) noexcept
    : Collider(Type::Sphere)
    , sphere(c, r)
{
}

void gearoenix::physics::collider::Sphere::set_location(const math::Vec3<double>& l) noexcept
{
    Collider::set_location(l);
    sphere.set_center(l);
}

void gearoenix::physics::collider::Sphere::local_scale(const double s) noexcept
{
    Collider::local_scale(s);
    sphere.set_radius(sphere.get_radius() * s);
}

void gearoenix::physics::collider::Sphere::local_x_scale(const double s) noexcept
{
    Collider::local_x_scale(s);
    GXLOGE("It is better to does not use sphere collider for a model that is going to be locally scale in X.")
}

std::optional<gearoenix::core::Real> gearoenix::physics::collider::Sphere::hit(const math::Ray3& r, const double d_min) const noexcept
{
    return sphere.hit(r, d_min);
}
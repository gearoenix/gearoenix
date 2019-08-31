#include "phs-cld-sphere.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../../math/math-ray.hpp"

gearoenix::physics::collider::Sphere::Sphere(const math::Vec3 &c, core::Real r) noexcept
    : Collider(Type::SPHERE)
    , sphere(c, r)
{}

std::optional<gearoenix::core::Real> gearoenix::physics::collider::Sphere::hit(const math::Ray3& r) const noexcept
{
    return sphere.hit(r);
}
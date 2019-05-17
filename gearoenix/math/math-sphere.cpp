#include "math-sphere.hpp"

gearoenix::math::Sphere::Sphere(const Vec3& position, const core::Real radius) noexcept
    : position(position)
    , radius(radius)
{
}

void gearoenix::math::Sphere::insert(const gearoenix::core::Real r) noexcept
{
    if (radius < r) {
        radius = r;
    }
}

gearoenix::math::IntersectionStatus::Type
gearoenix::math::Sphere::check_intersection(const gearoenix::math::Sphere& o) const noexcept
{
    const auto l = (position - o.position).length();
    const auto r = radius + o.radius;
    if (l > r)
        return IntersectionStatus::Out;
    if (l == r)
        return IntersectionStatus::Tangent;
    core::Real mnr, mxr;
    if (radius > o.radius) {
        mnr = o.radius;
        mxr = radius;
    } else {
        mnr = radius;
        mxr = o.radius;
    }
    if (l + mnr >= mxr)
        return IntersectionStatus::Cut;
    return IntersectionStatus::In;
}

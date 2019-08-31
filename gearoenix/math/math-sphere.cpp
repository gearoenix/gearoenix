#include "math-sphere.hpp"
#include "math-ray.hpp"

gearoenix::math::Sphere::Sphere(const Vec3& position, const core::Real radius) noexcept
    : position(position)
    , radius(radius)
    , radius2(radius * radius)
{
}

void gearoenix::math::Sphere::set_radius(const core::Real r) noexcept {
    radius2 = r * r;
    radius = r;
}

void gearoenix::math::Sphere::insert(const gearoenix::core::Real r) noexcept
{
    if (radius < r) {
        set_radius(r);
    }
}

std::optional<gearoenix::core::Real> gearoenix::math::Sphere::hit(const math::Ray3& r) noexcept
{
    const auto dir = position - r.origin;
    const auto dis = r.normalized_direction.dot(dir);
    if(dis <= radius) return std::nullopt;
    const auto dis2 = dis * dis;
    const auto dir2 = dir.dot(dir);
    if()
    radius2 - 
}

std::optional<gearoenix::core::Real> gearoenix::math::Sphere::hit(const math::Ray3& r, core::Real d_min) noexcept {

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

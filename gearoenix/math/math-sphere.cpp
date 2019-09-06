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

std::optional<gearoenix::core::Real> gearoenix::math::Sphere::hit(const math::Ray3& r) const noexcept
{
    const auto dir = position - r.get_origin();
	const auto dir2 = dir.square_length();
	if (dir2 <= radius2) return std::nullopt;
    const auto dis = r.get_normalized_direction().dot(dir);
	if (dis <= 0.0f) return std::nullopt;
    const auto dis2 = dis * dis;
    const auto a2 = dir2 - dis2;
    if(radius2 <= a2) return std::nullopt;
    return dis - std::sqrtf(radius2 - a2);
}

std::optional<gearoenix::core::Real> gearoenix::math::Sphere::hit(const math::Ray3& r, const core::Real d_min) const noexcept {
    if(const auto h = hit(r)) if((*h) < d_min) return h;
    return std::nullopt;
}

gearoenix::math::IntersectionStatus gearoenix::math::Sphere::check_intersection(const gearoenix::math::Sphere& o) const noexcept
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

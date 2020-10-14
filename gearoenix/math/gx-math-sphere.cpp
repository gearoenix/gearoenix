#include "gx-math-sphere.hpp"
#include "gx-math-ray.hpp"
#include <cmath>

gearoenix::math::Sphere::Sphere(const Vec3<double>& center, const double radius) noexcept
    : center(center)
    , radius(radius)
    , radius2(radius * radius)
{
}

void gearoenix::math::Sphere::set_radius(const double r) noexcept
{
    radius2 = r * r;
    radius = r;
}

void gearoenix::math::Sphere::set_center(const Vec3<double>& c) noexcept
{
    center = c;
}

void gearoenix::math::Sphere::insert(const double r) noexcept
{
    if (radius < r) {
        set_radius(r);
    }
}

std::optional<double> gearoenix::math::Sphere::hit(const math::Ray3& r) const noexcept
{
    const auto dir = center - r.get_origin();
    const auto dir2 = dir.square_length();
    if (dir2 <= radius2)
        return std::nullopt;
    const auto dis = r.get_normalized_direction().dot(dir);
    if (dis <= 0.0f)
        return std::nullopt;
    const auto dis2 = dis * dis;
    const auto a2 = dir2 - dis2;
    if (radius2 <= a2)
        return std::nullopt;
    return dis - std::sqrt(radius2 - a2);
}

std::optional<double> gearoenix::math::Sphere::hit(const math::Ray3& r, const double d_min) const noexcept
{
    if (const auto h = hit(r))
        if ((*h) < d_min)
            return h;
    return std::nullopt;
}

gearoenix::math::IntersectionStatus gearoenix::math::Sphere::check_intersection(const gearoenix::math::Sphere& o) const noexcept
{
    const auto l = (center - o.center).length();
    const auto r = radius + o.radius;
    if (l > r)
        return IntersectionStatus::Out;
    if (l == r)
        return IntersectionStatus::Tangent;
    double mnr, mxr;
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

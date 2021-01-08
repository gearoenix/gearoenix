#include "gx-math-aabb.hpp"
#include "gx-math-ray.hpp"
#include "gx-math-sphere.hpp"
#include <limits>

gearoenix::math::Aabb3::Aabb3() noexcept
    : upper(Vec3(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()))
    , lower(Vec3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()))
    , diameter(0.0)
    , center(0.0)
    , volume(0.0)
{
}

gearoenix::math::Aabb3::Aabb3(const Vec3<double>& u, const Vec3<double>& l) noexcept
    : upper(u)
    , lower(l)
    , diameter(u - l)
    , center((u + l) * 0.5)
    , volume(diameter.x * diameter.y * diameter.z)
{
}

gearoenix::math::Aabb3::Aabb3(const Vec3<double>& p) noexcept
    : upper(p)
    , lower(p)
    , diameter(0.0)
    , center(p)
    , volume(0.0)
{
}

void gearoenix::math::Aabb3::update() noexcept
{
    diameter = upper - lower;
    center = (upper + lower) * 0.5;
    volume = diameter.x * diameter.y * diameter.z;
}

void gearoenix::math::Aabb3::reset() noexcept
{
    upper = Vec3(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
    lower = Vec3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    diameter = Vec3(0.0, 0.0, 0.0);
    center = Vec3(0.0, 0.0, 0.0);
    volume = 0.0;
}

void gearoenix::math::Aabb3::reset(const Vec3<double>& p) noexcept
{
    upper = p;
    lower = p;
    diameter = Vec3(0.0);
    center = p;
    volume = 0.0;
}

void gearoenix::math::Aabb3::put(const Vec3<double>& p) noexcept
{
    put_without_update(p);
    update();
}

void gearoenix::math::Aabb3::put(const Sphere& o) noexcept
{
    const auto u = o.get_center() + o.get_radius();
    const auto l = o.get_center() - o.get_radius();

    upper = upper.maximum(u);
    lower = lower.minimum(l);

    update();
}

void gearoenix::math::Aabb3::put(const Aabb3& o) noexcept
{
    put_without_update(o);
    update();
}

void gearoenix::math::Aabb3::put_without_update(const Vec3<double>& p) noexcept
{
    upper = upper.maximum(p);
    lower = lower.minimum(p);
}

void gearoenix::math::Aabb3::put_without_update(const Aabb3& o) noexcept
{
    upper = upper.maximum(o.upper);
    lower = lower.minimum(o.lower);
}

bool gearoenix::math::Aabb3::check_intersection(const Aabb3& o, Aabb3& intersection) const noexcept
{
    int equals = 0;
    const auto u = upper.minimum(o.upper);
    const auto l = lower.maximum(o.lower);
    for (int i = 0; i < 3; ++i) {
        const auto uv = u[i];
        const auto lv = l[i];
        if (uv == lv) {
            ++equals;
        } else if (lv > uv) {
            return false;
        }
    }
    if (equals < 3) {
        intersection = Aabb3(u, l);
        return true;
    }
    return false;
}

bool gearoenix::math::Aabb3::check_intersection(const Aabb3& o) const noexcept
{
    // o.upper > lower
    // o.lower < upper
    return lower.x < o.upper.x
        && upper.x > o.lower.x
        && lower.y < o.upper.y
        && upper.y > o.lower.y
        && lower.z < o.upper.z
        && upper.z > o.lower.z;
}

bool gearoenix::math::Aabb3::check_intersection(const gearoenix::math::Sphere& o) const noexcept
{
    return check_intersection(Aabb3(o.get_center() + o.get_radius(), o.get_center() - o.get_radius()));
}

std::optional<double> gearoenix::math::Aabb3::hit(const math::Ray3& r, const double d_min) const noexcept
{
    const auto& ro = r.get_origin();
    const auto& rrd = r.get_reversed_normalized_direction();
    const auto t0 = (lower - ro) * rrd;
    const auto t1 = (upper - ro) * rrd;
    const auto t_min = t0.minimum(t1).maximum();
    const auto t_max = t0.maximum(t1).minimum();
    if (t_max >= 0.0 && t_min <= t_max && t_min < d_min)
        return t_min;
    return std::nullopt;
}

gearoenix::math::IntersectionStatus gearoenix::math::Aabb3::check_intersection_status(const Aabb3& o) const noexcept
{
    if (upper.x > o.upper.x
        && upper.y > o.upper.y
        && upper.z > o.upper.z
        && lower.x < o.lower.x
        && lower.y < o.lower.y
        && lower.z < o.lower.z)
        return IntersectionStatus::In;
    if (check_intersection(o))
        return IntersectionStatus::Cut;
    return IntersectionStatus::Out;
}

void gearoenix::math::Aabb3::set_center(const Vec3<double>& c) noexcept
{
    const auto t = c - center;
    center = c;
    upper += t;
    lower += t;
}

void gearoenix::math::Aabb3::set_diameter(const Vec3<double>& d) noexcept
{
    diameter = d;
    const auto hd = d * 0.5;
    upper = center + hd;
    lower = center - hd;
    volume = d.x * d.y * d.z;
}

void gearoenix::math::Aabb3::read(platform::stream::Stream& s) noexcept
{
    upper.read(s);
    lower.read(s);
    update();
}

void gearoenix::math::Aabb3::get_all_corners(math::Vec3<double> (&corners)[8]) const noexcept
{
    corners[0].x = upper.x;
    corners[0].y = upper.y;
    corners[0].z = upper.z;

    corners[1].x = lower.x;
    corners[1].y = upper.y;
    corners[1].z = upper.z;

    corners[2].x = upper.x;
    corners[2].y = lower.y;
    corners[2].z = upper.z;

    corners[3].x = lower.x;
    corners[3].y = lower.y;
    corners[3].z = upper.z;

    corners[4].x = upper.x;
    corners[4].y = upper.y;
    corners[4].z = lower.z;

    corners[5].x = lower.x;
    corners[5].y = upper.y;
    corners[5].z = lower.z;

    corners[6].x = upper.x;
    corners[6].y = lower.y;
    corners[6].z = lower.z;

    corners[7].x = lower.x;
    corners[7].y = lower.y;
    corners[7].z = lower.z;
}

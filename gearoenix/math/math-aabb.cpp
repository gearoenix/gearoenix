#include "math-aabb.hpp"
#include "../system/stream/sys-stm-stream.hpp"
#include "math-ray.hpp"
#include "math-sphere.hpp"
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

    upper.x = GX_MAX(u.x, upper.x);
    upper.y = GX_MAX(u.y, upper.y);
    upper.z = GX_MAX(u.z, upper.z);
    lower.x = GX_MIN(l.x, lower.x);
    lower.y = GX_MIN(l.y, lower.y);
    lower.z = GX_MIN(l.z, lower.z);

    update();
}

void gearoenix::math::Aabb3::put(const Aabb3& o) noexcept
{
    put_without_update(o);
    update();
}

void gearoenix::math::Aabb3::put_without_update(const Vec3<double>& p) noexcept
{
    if (p.x > upper.x) {
        upper.x = p.x;
    } else if (p.x < lower.x) {
        lower.x = p.x;
    }

    if (p.y > upper.y) {
        upper.y = p.y;
    } else if (p.y < lower.y) {
        lower.y = p.y;
    }

    if (p.z > upper.z) {
        upper.z = p.z;
    } else if (p.z < lower.z) {
        lower.z = p.z;
    }
}

void gearoenix::math::Aabb3::put_without_update(const Aabb3& o) noexcept
{
    upper.x = GX_MAX(o.upper.x, upper.x);
    upper.y = GX_MAX(o.upper.y, upper.y);
    upper.z = GX_MAX(o.upper.z, upper.z);
    lower.x = GX_MIN(o.lower.x, lower.x);
    lower.y = GX_MIN(o.lower.y, lower.y);
    lower.z = GX_MIN(o.lower.z, lower.z);
}

bool gearoenix::math::Aabb3::check_intersection(const Aabb3& o, Aabb3& intersection) const noexcept
{
    int equals = 0;
    for (int i = 0; i < 3; ++i) {
        const double eb = lower[i];
        const double oeb = o.lower[i];
        const double ieb = GX_MAX(eb, oeb);
        const double ea = upper[i];
        const double oea = o.upper[i];
        const double iea = GX_MIN(ea, oea);
        intersection.upper[i] = iea;
        intersection.lower[i] = ieb;
        if (ieb == iea) {
            ++equals;
        } else if (ieb > iea) {
            return false;
        }
    }
    return equals < 3;
}

bool gearoenix::math::Aabb3::check_intersection(const Aabb3& o) const noexcept
{
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
    const auto tmin = t0.minimum(t1).maximum();
    const auto tmax = t0.maximum(t1).minimum();
    if (tmax >= 0.0 && tmin <= tmax && tmin < d_min)
        return tmin;
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

void gearoenix::math::Aabb3::read(system::stream::Stream* const s) noexcept
{
    upper.read(s);
    lower.read(s);
    update();
}

void gearoenix::math::Aabb3::get_all_corners(math::Vec3<double> (&corners)[8]) const noexcept
{
    corners[0][0] = upper.x;
    corners[0][1] = upper.y;
    corners[0][2] = upper.z;

    corners[1][0] = lower.x;
    corners[1][1] = upper.y;
    corners[1][2] = upper.z;

    corners[2][0] = upper.x;
    corners[2][1] = lower.y;
    corners[2][2] = upper.z;

    corners[3][0] = lower.x;
    corners[3][1] = lower.y;
    corners[3][2] = upper.z;

    corners[4][0] = upper.x;
    corners[4][1] = upper.y;
    corners[4][2] = lower.z;

    corners[5][0] = lower.x;
    corners[5][1] = upper.y;
    corners[5][2] = lower.z;

    corners[6][0] = upper.x;
    corners[6][1] = lower.y;
    corners[6][2] = lower.z;

    corners[7][0] = lower.x;
    corners[7][1] = lower.y;
    corners[7][2] = lower.z;
}

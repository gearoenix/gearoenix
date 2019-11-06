#include "math-aabb.hpp"
#include "../core/cr-static.hpp"
#include "../system/stream/sys-stm-stream.hpp"
#include "math-ray.hpp"
#include "math-sphere.hpp"
#include <limits>

gearoenix::math::Aabb3::Aabb3() noexcept
    : upper(Vec3(-std::numeric_limits<core::Real>::max(), -std::numeric_limits<core::Real>::max(), -std::numeric_limits<core::Real>::max()))
    , lower(Vec3(std::numeric_limits<core::Real>::max(), std::numeric_limits<core::Real>::max(), std::numeric_limits<core::Real>::max()))
    , diameter(0.0f, 0.0f, 0.0f)
    , center(0.0f, 0.0f, 0.0f)
    , volume(0.0f)
{
}

gearoenix::math::Aabb3::Aabb3(const Vec3& u, const Vec3& l) noexcept
    : upper(u)
    , lower(l)
    , diameter(u - l)
    , center((u + l) * 0.5f)
    , volume(diameter[0] * diameter[1] * diameter[2])
{
}

gearoenix::math::Aabb3::Aabb3(const Vec3& p) noexcept
    : upper(p)
    , lower(p)
    , diameter(Vec3(0.0f))
    , center(p)
    , volume(0.0f)
{
}

void gearoenix::math::Aabb3::update() noexcept
{
    diameter = upper - lower;
    center = (upper + lower) * 0.5f;
    volume = diameter[0] * diameter[1] * diameter[2];
}

void gearoenix::math::Aabb3::reset() noexcept
{
    upper = Vec3(-std::numeric_limits<core::Real>::max(), -std::numeric_limits<core::Real>::max(), -std::numeric_limits<core::Real>::max());
    lower = Vec3(std::numeric_limits<core::Real>::max(), std::numeric_limits<core::Real>::max(), std::numeric_limits<core::Real>::max());
    diameter = Vec3(0.0f, 0.0f, 0.0f);
    center = Vec3(0.0f, 0.0f, 0.0f);
    volume = diameter[0] * diameter[1] * diameter[2];
}

void gearoenix::math::Aabb3::reset(const Vec3& p) noexcept
{
    upper = p;
    lower = p;
    diameter = Vec3(0.0f, 0.0f, 0.0f);
    center = p;
    volume = 0.0f;
}

void gearoenix::math::Aabb3::put(const Vec3& p) noexcept
{
    put_without_update(p);
    update();
}

void gearoenix::math::Aabb3::put(const Sphere& o) noexcept
{
    const auto u = o.get_center() + o.get_radius();
    const auto l = o.get_center() - o.get_radius();

    upper[0] = GX_MAX(u[0], upper[0]);
    upper[1] = GX_MAX(u[1], upper[1]);
    upper[2] = GX_MAX(u[2], upper[2]);
    lower[0] = GX_MIN(l[0], lower[0]);
    lower[1] = GX_MIN(l[1], lower[1]);
    lower[2] = GX_MIN(l[2], lower[2]);

    update();
}

void gearoenix::math::Aabb3::put(const gearoenix::math::Aabb3& o) noexcept
{
    put_without_update(o);
    update();
}

void gearoenix::math::Aabb3::put_without_update(const Vec3& p) noexcept
{
    for (int i = 0; i < 3; ++i) {
        if (p[i] > upper[i]) {
            upper[i] = p[i];
        } else if (p[i] < lower[i]) {
            lower[i] = p[i];
        }
    }
}

void gearoenix::math::Aabb3::put_without_update(const Aabb3& o) noexcept
{
    upper[0] = GX_MAX(o.upper[0], upper[0]);
    upper[1] = GX_MAX(o.upper[1], upper[1]);
    upper[2] = GX_MAX(o.upper[2], upper[2]);
    lower[0] = GX_MIN(o.lower[0], lower[0]);
    lower[1] = GX_MIN(o.lower[1], lower[1]);
    lower[2] = GX_MIN(o.lower[2], lower[2]);
}

bool gearoenix::math::Aabb3::check_intersection(const Aabb3& o, Aabb3& intersection) const noexcept
{
    int equals = 0;
    for (int i = 0; i < 3; ++i) {
        const core::Real eb = lower[i];
        const core::Real oeb = o.lower[i];
        const core::Real ieb = GX_MAX(eb, oeb);
        const core::Real ea = upper[i];
        const core::Real oea = o.upper[i];
        const core::Real iea = GX_MIN(ea, oea);
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
    return lower[0] < o.upper[0]
        && upper[0] > o.lower[0]
        && lower[1] < o.upper[1]
        && upper[1] > o.lower[1]
        && lower[2] < o.upper[2]
        && upper[2] > o.lower[2];
}

bool gearoenix::math::Aabb3::check_intersection(const gearoenix::math::Sphere& o) const noexcept
{
    return check_intersection(Aabb3(o.get_center() + o.get_radius(), o.get_center() - o.get_radius()));
}

std::optional<gearoenix::core::Real> gearoenix::math::Aabb3::hit(const math::Ray3& r, const core::Real d_min) const noexcept
{
    const Vec3& ro = r.get_origin();
    const Vec3& rrd = r.get_reversed_normalized_direction();
    const Vec3 t0 = (lower - ro) * rrd;
    const Vec3 t1 = (upper - ro) * rrd;
    const Vec3 tsmall = t0.minimum(t1);
    const Vec3 tbig = t0.maximum(t1);
    const core::Real tmin = tsmall.maximum();
    const core::Real tmax = tbig.minimum();
    if (tmin < tmax && tmin < d_min)
        return tmin;
    return std::nullopt;
}

gearoenix::math::IntersectionStatus gearoenix::math::Aabb3::check_intersection_status(const Aabb3& o) const noexcept
{
    if (upper[0] > o.upper[0]
        && upper[1] > o.upper[1]
        && upper[2] > o.upper[2]
        && lower[0] < o.lower[0]
        && lower[1] < o.lower[1]
        && lower[2] < o.lower[2])
        return IntersectionStatus::In;
    if (lower[0] < o.upper[0]
        && upper[0] > o.lower[0]
        && lower[1] < o.upper[1]
        && upper[1] > o.lower[1]
        && lower[2] < o.upper[2]
        && upper[2] > o.lower[2])
        return IntersectionStatus::Cut;
    return IntersectionStatus::Out;
}

void gearoenix::math::Aabb3::set_center(const Vec3& c) noexcept
{
    Vec3 t = c - center;
    center = c;
    upper += t;
    lower += t;
}

void gearoenix::math::Aabb3::set_diameter(const Vec3& d) noexcept
{
    diameter = d;
    const auto hd = d * 0.5f;
    upper = center + hd;
    lower = center - hd;
    volume = d[0] * d[1] * d[2];
}

void gearoenix::math::Aabb3::read(system::stream::Stream* s) noexcept
{
    upper.read(s);
    lower.read(s);
    update();
}

void gearoenix::math::Aabb3::get_all_corners(math::Vec3 (&corners)[8]) const noexcept
{
    corners[0][0] = upper[0];
    corners[0][1] = upper[1];
    corners[0][2] = upper[2];

    corners[1][0] = lower[0];
    corners[1][1] = upper[1];
    corners[1][2] = upper[2];

    corners[2][0] = upper[0];
    corners[2][1] = lower[1];
    corners[2][2] = upper[2];

    corners[3][0] = lower[0];
    corners[3][1] = lower[1];
    corners[3][2] = upper[2];

    corners[4][0] = upper[0];
    corners[4][1] = upper[1];
    corners[4][2] = lower[2];

    corners[5][0] = lower[0];
    corners[5][1] = upper[1];
    corners[5][2] = lower[2];

    corners[6][0] = upper[0];
    corners[6][1] = lower[1];
    corners[6][2] = lower[2];

    corners[7][0] = lower[0];
    corners[7][1] = lower[1];
    corners[7][2] = lower[2];
}

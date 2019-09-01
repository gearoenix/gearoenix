#include "math-aabb.hpp"
#include "../core/cr-build-configuration.hpp"
#include "../core/cr-static.hpp"
#include "math-ray.hpp"
#include "math-sphere.hpp"
#include <limits>

void gearoenix::math::Aabb3::update() noexcept
{
    diameter = upper - lower;
    center = (upper + lower) * 0.5f;
    volume = diameter[0] * diameter[1] * diameter[2];
}

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
    for (int i = 0; i < 3; ++i) {
        if (p[i] > upper[i]) {
            upper[i] = p[i];
        } else if (p[i] < lower[i]) {
            lower[i] = p[i];
        }
    }
    update();
}

void gearoenix::math::Aabb3::put(const Sphere& o) noexcept
{
    const auto u = o.get_position() + o.get_radius();
    const auto l = o.get_position() - o.get_radius();

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
    upper[0] = GX_MAX(o.upper[0], upper[0]);
    upper[1] = GX_MAX(o.upper[1], upper[1]);
    upper[2] = GX_MAX(o.upper[2], upper[2]);
    lower[0] = GX_MIN(o.lower[0], lower[0]);
    lower[1] = GX_MIN(o.lower[1], lower[1]);
    lower[2] = GX_MIN(o.lower[2], lower[2]);

    update();
}

bool gearoenix::math::Aabb3::test(const Ray3& ray, core::Real& t_min_result) const noexcept
{
    const math::Vec3& ro = ray.get_origin();
    const math::Vec3& rd = ray.get_normalized_direction();
    core::Real t_max = std::numeric_limits<core::Real>::max();
    core::Real t_min = -t_max;
    for (int i = 0; i < 3; ++i) {
        if (GX_IS_ZERO(rd[i]))
            continue;
        const core::Real oor = upper[i] - ro[i];
        const core::Real oomr = lower[i] - ro[i];
        const core::Real rrd = 1.0f / rd[i];
        const core::Real f1 = oor * rrd;
        const core::Real f2 = oomr * rrd;
        const core::Real upperf = GX_MAX(f1, f2);
        const core::Real lowerf = GX_MIN(f1, f2);
        t_max = GX_MIN(t_max, upperf);
        t_min = GX_MAX(t_min, lowerf);
    }
    if ((t_max >= t_min) && t_max > 0.0f) {
        if (t_min_result > t_min) {
            t_min_result = t_min;
            return true;
        }
    }

    return false;
}

bool gearoenix::math::Aabb3::test(const Aabb3& o, Aabb3& intersection) const noexcept
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

bool gearoenix::math::Aabb3::test(const Aabb3& o) const noexcept
{
    return lower[0] < o.upper[0]
        && upper[0] > o.lower[0]
        && lower[1] < o.upper[1]
        && upper[1] > o.lower[1]
        && lower[2] < o.upper[2]
        && upper[2] > o.lower[2];
}

bool gearoenix::math::Aabb3::test(const gearoenix::math::Sphere& o) const noexcept
{
    return test(Aabb3(o.get_position() + o.get_radius(), o.get_position() - o.get_radius()));
}

gearoenix::math::IntersectionStatus::Type gearoenix::math::Aabb3::check_intersection(const Aabb3& o) const noexcept
{
    if (upper[0] > o.upper[0]
        && upper[1] > o.upper[1]
        && upper[2] > o.upper[2]
        && lower[0] < o.lower[0]
        && lower[1] < o.lower[1]
        && lower[2] < o.lower[2])
        return gearoenix::math::IntersectionStatus::In;
    if (lower[0] < o.upper[0]
        && upper[0] > o.lower[0]
        && lower[1] < o.upper[1]
        && upper[1] > o.lower[1]
        && lower[2] < o.upper[2]
        && upper[2] > o.lower[2])
        return gearoenix::math::IntersectionStatus::Cut;
    return gearoenix::math::IntersectionStatus::Out;
}

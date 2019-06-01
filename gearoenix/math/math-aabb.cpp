#include "math-aabb.hpp"
#include "../core/cr-build-configuration.hpp"
#include "../core/cr-static.hpp"
#include "math-ray.hpp"
#include "math-sphere.hpp"
#include <limits>

gearoenix::math::Aabb3::Aabb3() noexcept
    : mx(Vec3(-std::numeric_limits<core::Real>::max(), -std::numeric_limits<core::Real>::max(), -std::numeric_limits<core::Real>::max()))
    , mn(Vec3(std::numeric_limits<core::Real>::max(), std::numeric_limits<core::Real>::max(), std::numeric_limits<core::Real>::max()))
{
}

gearoenix::math::Aabb3::Aabb3(const Vec3& mx, const Vec3& mn) noexcept
    : mx(mx)
    , mn(mn)
{
#ifdef GX_DEBUG_MODE
    //if()
#endif
}

void gearoenix::math::Aabb3::reset() noexcept
{
    mx = Vec3(-std::numeric_limits<core::Real>::max(), -std::numeric_limits<core::Real>::max(), -std::numeric_limits<core::Real>::max());
    mn = Vec3(std::numeric_limits<core::Real>::max(), std::numeric_limits<core::Real>::max(), std::numeric_limits<core::Real>::max());
}

void gearoenix::math::Aabb3::reset(const Vec3& p) noexcept
{
    mx = p;
    mn = p;
}

void gearoenix::math::Aabb3::put(const Vec3& p) noexcept
{
    for (int i = 0; i < 3; ++i) {
        if (p[i] > mx[i]) {
            mx[i] = p[i];
        } else if (p[i] < mn[i]) {
            mn[i] = p[i];
        }
    }
}

void gearoenix::math::Aabb3::put(const Sphere& o) noexcept
{
    put(o.position + o.radius);
    put(o.position - o.radius);
}

bool gearoenix::math::Aabb3::test(const Ray3& ray, core::Real& t_min_result) const noexcept
{
    const math::Vec3& ro = ray.get_origin();
    const math::Vec3& rd = ray.get_direction();
    core::Real t_max = std::numeric_limits<core::Real>::max();
    core::Real t_min = -t_max;
    for (int i = 0; i < 3; ++i) {
        if (GX_IS_ZERO(rd[i]))
            continue;
        const core::Real oor = mx[i] - ro[i];
        const core::Real oomr = mn[i] - ro[i];
        const core::Real rrd = 1.0f / rd[i];
        const core::Real f1 = oor * rrd;
        const core::Real f2 = oomr * rrd;
        const core::Real mxf = GX_MAX(f1, f2);
        const core::Real mnf = GX_MIN(f1, f2);
        t_max = GX_MIN(t_max, mxf);
        t_min = GX_MAX(t_min, mnf);
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
        const core::Real eb = mn[i];
        const core::Real oeb = o.mn[i];
        const core::Real ieb = GX_MAX(eb, oeb);
        const core::Real ea = mx[i];
        const core::Real oea = o.mx[i];
        const core::Real iea = GX_MIN(ea, oea);
        intersection.mx[i] = iea;
        intersection.mn[i] = ieb;
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
    return mn[0] < o.mx[0]
        && mx[0] > o.mn[0]
        && mn[1] < o.mx[1]
        && mx[1] > o.mn[1]
        && mn[2] < o.mx[2]
        && mx[2] > o.mn[2];
}

bool gearoenix::math::Aabb3::test(const gearoenix::math::Sphere& o) const noexcept
{
    return test(Aabb3(o.position + o.radius, o.position - o.radius));
}

gearoenix::math::IntersectionStatus::Type gearoenix::math::Aabb3::check_intersection(const Aabb3& o) const noexcept
{
    if (mx[0] > o.mx[0]
        && mx[1] > o.mx[1]
        && mx[2] > o.mx[2]
        && mn[0] < o.mn[0]
        && mn[1] < o.mn[1]
        && mn[2] < o.mn[2])
        return gearoenix::math::IntersectionStatus::In;
    if (mn[0] < o.mx[0]
        && mx[0] > o.mn[0]
        && mn[1] < o.mx[1]
        && mx[1] > o.mn[1]
        && mn[2] < o.mx[2]
        && mx[2] > o.mn[2])
        return gearoenix::math::IntersectionStatus::Cut;
    return gearoenix::math::IntersectionStatus::Out;
}

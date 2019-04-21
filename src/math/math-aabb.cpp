#include "math-aabb.hpp"
#include "../core/cr-static.hpp"
#include "math-ray.hpp"
#include <limits>

gearoenix::math::Aabb3::Aabb3() {}

gearoenix::math::Aabb3::Aabb3(const Vec3& max, const Vec3& min)
    : a(max)
    , b(min)
{
}

void gearoenix::math::Aabb3::reset(const Vec3& p)
{
    for (int i = 0; i < 3; ++i) {
        a[i] = p[i];
        b[i] = p[i];
    }
}

void gearoenix::math::Aabb3::put(const Vec3& p)
{
    for (int i = 0; i < 3; ++i)
        if (p[i] > a[i])
            a[i] = p[i];
        else if (p[i] < b[i])
            b[i] = p[i];
}

bool gearoenix::math::Aabb3::test(const Ray3& ray, core::Real& tmin_result) const
{
    const math::Vec3& ro = ray.get_origin();
    const math::Vec3& rd = ray.get_direction();
    core::Real tmax = std::numeric_limits<core::Real>::max();
    core::Real tmin = -tmax;
    for (int i = 0; i < 3; ++i) {
        if (GXISZERO(rd[i]))
            continue;
        const core::Real oor = a[i] - ro[i];
        const core::Real oomr = b[i] - ro[i];
        const core::Real rrd = 1.0f / rd[i];
        const core::Real f1 = oor * rrd;
        const core::Real f2 = oomr * rrd;
        const core::Real mxf = GXMAX(f1, f2);
        const core::Real mnf = GXMIN(f1, f2);
        tmax = GXMIN(tmax, mxf);
        tmin = GXMAX(tmin, mnf);
    }
    if ((tmax >= tmin) && tmax > 0.0f) {
        if (tmin_result > tmin) {
            tmin_result = tmin;
            return true;
        }
    }

    return false;
}

bool gearoenix::math::Aabb3::test(const Aabb3& o, Aabb3& intersection) const
{
    int equals = 0;
    for (int i = 0; i < 3; ++i) {
        const core::Real eb = b[i];
        const core::Real oeb = o.b[i];
        const core::Real ieb = GXMAX(eb, oeb);
        const core::Real ea = a[i];
        const core::Real oea = o.a[i];
        const core::Real iea = GXMIN(ea, oea);
        intersection.a[i] = iea;
        intersection.b[i] = ieb;
        if (ieb == iea) {
            ++equals;
        } else if (ieb > iea) {
            return false;
        }
    }
    return equals < 3;
}

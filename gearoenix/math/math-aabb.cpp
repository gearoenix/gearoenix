#include "math-aabb.hpp"
#include "../core/cr-static.hpp"
#include "math-ray.hpp"
#include "../core/cr-build-configuration.hpp"
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

void gearoenix::math::Aabb3::reset(const Vec3& p)noexcept
{
	mx = p;
    mn = p;
}

void gearoenix::math::Aabb3::put(const Vec3& p)noexcept
{
	for (int i = 0; i < 3; ++i)
	{
		if (p[i] > mx[i])
		{
			mx[i] = p[i];
		}
		else if (p[i] < mn[i])
		{
			mn[i] = p[i];
		}
	}
}

bool gearoenix::math::Aabb3::test(const Ray3& ray, core::Real& tmin_result) const noexcept
{
    const math::Vec3& ro = ray.get_origin();
    const math::Vec3& rd = ray.get_direction();
    core::Real tmax = std::numeric_limits<core::Real>::max();
    core::Real tmin = -tmax;
    for (int i = 0; i < 3; ++i) {
        if (GXISZERO(rd[i]))
            continue;
        const core::Real oor = mx[i] - ro[i];
        const core::Real oomr = mn[i] - ro[i];
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

bool gearoenix::math::Aabb3::test(const Aabb3& o, Aabb3& intersection) const noexcept
{
	int equals = 0;
	for (int i = 0; i < 3; ++i) {
		const core::Real eb = mn[i];
		const core::Real oeb = o.mn[i];
		const core::Real ieb = GXMAX(eb, oeb);
		const core::Real ea = mx[i];
		const core::Real oea = o.mx[i];
		const core::Real iea = GXMIN(ea, oea);
		intersection.mx[i] = iea;
		intersection.mn[i] = ieb;
		if (ieb == iea) {
			++equals;
		}
		else if (ieb > iea) {
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

gearoenix::math::IntersectionStatus::Type gearoenix::math::Aabb3::check_intersection(const Aabb3& o) const noexcept
{
	if (mx[0] > o.mx[0]
		&& mx[1] > o.mx[1]
		&& mx[2] > o.mx[2]
		&& mn[0] < o.mn[0]
		&& mn[1] < o.mn[1]
		&& mn[2] < o.mn[2]) return gearoenix::math::IntersectionStatus::In;
	if (mn[0] < o.mx[0]
		&& mx[0] > o.mn[0]
		&& mn[1] < o.mx[1]
		&& mx[1] > o.mn[1]
		&& mn[2] < o.mx[2]
		&& mx[2] > o.mn[2]) return gearoenix::math::IntersectionStatus::Cut;
	return gearoenix::math::IntersectionStatus::Out;
}

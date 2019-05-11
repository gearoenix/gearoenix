#include "math-frustum.hpp"
#include "math-sphere.hpp"
#include "../system/sys-log.hpp"

gearoenix::math::Frustum::Frustum(const Plane (&planes)[6]) noexcept
	: planes { planes[0], planes[1], planes[2], planes[3], planes[4], planes[5] }
{

}

gearoenix::math::IntersectionStatus::Type gearoenix::math::Frustum::check_intersection(const Sphere& s) const noexcept
{
	IntersectionStatus::Type state = IntersectionStatus::In;
	for (const Plane& p : planes) {
		const core::Real sd = s.position.dot(p.normal) + p.d;
		if(s.radius <= -sd) return IntersectionStatus::Out;
		if (s.radius > sd) state = IntersectionStatus::Cut;
	}
	return state;
}

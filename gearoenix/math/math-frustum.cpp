#include "math-frustum.hpp"
#include "math-sphere.hpp"
#include "../system/sys-log.hpp"

gearoenix::math::Frustum::Frustum(Plane (&planes)[6])
	: planes { planes[0], planes[1], planes[2], planes[3], planes[4], planes[5] }
{

}

gearoenix::math::IntersectionStatus::Type gearoenix::math::Frustum::check_intersection(const Sphere& s) const noexcept
{
	IntersectionStatus::Type state = IntersectionStatus::Out;
	for (const Plane& p : planes) {
		const IntersectionStatus::Type c = p.check_intersection(s);
		if (c == IntersectionStatus::Above) return IntersectionStatus::Out;
		switch (state) {
		case IntersectionStatus::Tangent:
			continue;
		case IntersectionStatus::Cut:
			if (c == IntersectionStatus::Tangent) state = IntersectionStatus::Tangent;
			break;
		case IntersectionStatus::In:
			if (c == IntersectionStatus::Tangent) state = IntersectionStatus::Tangent;
			else if (c == IntersectionStatus::Cut) state = IntersectionStatus::Cut;
			break;
		case IntersectionStatus::Out:
			if (c == IntersectionStatus::Tangent) state = IntersectionStatus::Tangent;
			else if (c == IntersectionStatus::Cut) state = IntersectionStatus::Cut;
			else if (c == IntersectionStatus::Under) state = IntersectionStatus::In;
			break;
		default:
			GXUNEXPECTED;
		}
	}
	return state;
}

#ifndef GEAROEMIX_MATH_FRUSTUM_HPP
#define GEAROEMIX_MATH_FRUSTUM_HPP
#include "math-plane.hpp"
namespace gearoenix
{
	namespace math
	{
		struct Frustum {
			Plane planes[6];
			
			Frustum(Plane (&planes)[6]);

			IntersectionStatus::Type check_intersection(const Sphere& s) const noexcept;
		};
	}
}
#endif
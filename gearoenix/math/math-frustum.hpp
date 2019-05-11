#ifndef GEAROEMIX_MATH_FRUSTUM_HPP
#define GEAROEMIX_MATH_FRUSTUM_HPP
#include "math-plane.hpp"
namespace gearoenix
{
	namespace math
	{
		struct Frustum {
			Plane planes[6];
			
			Frustum() = delete;
			explicit Frustum(const Plane (&planes)[6]) noexcept;

			IntersectionStatus::Type check_intersection(const Sphere& s) const noexcept;
		};
	}
}
#endif
#ifndef GEAROEMIX_MATH_PLANE_HPP
#define GEAROEMIX_MATH_PLANE_HPP
#include "math-vector.hpp"
#include "math-intersection-status.hpp"
namespace gearoenix
{
	namespace math
	{
		struct Sphere;
		struct Plane
		{
			Vec3 normal;
			core::Real d;
			Vec3 point;

			Plane() = delete;
			explicit Plane(const Vec3& point, const Vec3& normal) noexcept;

			IntersectionStatus::Type check_intersection(const Sphere& s) const noexcept;
		};
	}
}
#endif
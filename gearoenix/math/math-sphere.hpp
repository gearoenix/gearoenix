#ifndef GEAROEMIX_MATH_SPHERE_HPP
#define GEAROEMIX_MATH_SPHERE_HPP
#include "math-vector.hpp"
#include "math-intersection-status.hpp"
namespace gearoenix
{
	namespace math
	{
		struct Sphere {
			Vec3 position;
			core::Real radius;

			Sphere() = delete;
			explicit Sphere(const Vec3& position, const core::Real radius) noexcept;

			IntersectionStatus::Type check_intersection(const Sphere& o) const noexcept;
		};
	}
}
#endif
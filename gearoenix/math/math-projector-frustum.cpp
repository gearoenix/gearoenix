#include "math-projector-frustum.hpp"
#include "math-sphere.hpp"
#include "math-aabb.hpp"

gearoenix::math::ProjectorFrustum::ProjectorFrustum(const Mat4x4& view_projection) noexcept
	: view_projection(view_projection)
{
}

gearoenix::math::IntersectionStatus::Type gearoenix::math::ProjectorFrustum::check_intersection(const Sphere& s) const noexcept
{
	return check_intersection({
		s.position + Vec3(s.radius, s.radius, s.radius),
		s.position + Vec3(-s.radius, s.radius, s.radius),
		s.position + Vec3(s.radius, -s.radius, s.radius),
		s.position + Vec3(-s.radius, -s.radius, s.radius),
		s.position + Vec3(s.radius, s.radius, -s.radius),
		s.position + Vec3(-s.radius, s.radius, -s.radius),
		s.position + Vec3(s.radius, -s.radius, -s.radius),
		s.position + Vec3(-s.radius, -s.radius, -s.radius),
	});
}

gearoenix::math::IntersectionStatus::Type gearoenix::math::ProjectorFrustum::check_intersection(const Aabb3& aabb) const noexcept
{
	const Vec3 c = (aabb.mx + aabb.mn) * 0.5f;
	const Vec3 r = (aabb.mx - aabb.mn) * 0.5f;
	return check_intersection({
		aabb.mx,
		c + Vec3(-r[0], r[1], r[2]),
		c + Vec3(r[0], -r[1], r[2]),
		c + Vec3(-r[0], -r[1], r[2]),
		c + Vec3(r[0], r[1], -r[2]),
		c + Vec3(-r[0], r[1], -r[2]),
		c + Vec3(r[0], -r[1], -r[2]),
		aabb.mn,
		});
}

gearoenix::math::IntersectionStatus::Type gearoenix::math::ProjectorFrustum::check_intersection(const Vec3(&points)[8]) const noexcept
{

	return IntersectionStatus::Out;
}

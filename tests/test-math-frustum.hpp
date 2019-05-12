#include <gearoenix/math/math-aabb.hpp>
#include <gearoenix/math/math-frustum.hpp>
#include <gearoenix/math/math-projector-frustum.hpp>
#include <gearoenix/math/math-sphere.hpp>

BOOST_AUTO_TEST_CASE(math_frustum_test)
{
	namespace gxm = gearoenix::math;

	const gxm::Mat4x4 identity;
	
	const gxm::Frustum f({
		gxm::Plane(gxm::Vec3(-1.0f, -1.0f, -1.0f), gxm::Vec3(1.0f, 0.0f, 0.0f)),
		gxm::Plane(gxm::Vec3(-1.0f, -1.0f, -1.0f), gxm::Vec3(0.0f, 1.0f, 0.0f)),
		gxm::Plane(gxm::Vec3(-1.0f, -1.0f, -1.0f), gxm::Vec3(0.0f, 0.0f, 1.0f)),
		gxm::Plane(gxm::Vec3(1.0f, 1.0f, 1.0f), gxm::Vec3(-1.0f, 0.0f, 0.0f)),
		gxm::Plane(gxm::Vec3(1.0f, 1.0f, 1.0f), gxm::Vec3(0.0f, -1.0f, 0.0f)),
		gxm::Plane(gxm::Vec3(1.0f, 1.0f, 1.0f), gxm::Vec3(0.0f, 0.0f, -1.0f)),
	});
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(0.0f, 0.0f, 0.0f), 0.9999f)), gxm::IntersectionStatus::In);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.0f, 1.0f, 1.0f), 1.0f)), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.5f, 1.5f, 1.5f), 1.0f)), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.6f, 1.6f, 1.6f), 1.0f)), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.7f, 1.7f, 1.7f), 1.0f)), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(2.001f, 2.001f, 2.001f), 1.0f)), gxm::IntersectionStatus::Out);
	
	const gxm::ProjectorFrustum pf(identity);
	BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Aabb3(gxm::Vec3(0.999f, 0.999f, 0.999f), gxm::Vec3(-0.999f, -0.999f, -0.999f))), gxm::IntersectionStatus::In);
	BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Aabb3(gxm::Vec3(2.0f, 2.0f, 2.0f), gxm::Vec3(0.0f, 0.0f, 0.0f))), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Aabb3(gxm::Vec3(2.0f, 2.0f, 2.0f), gxm::Vec3(1.0f, 1.0f, 1.0f))), gxm::IntersectionStatus::Out);
	BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Sphere(gxm::Vec3(0.0f, 0.0f, 0.0f), 0.999f)), gxm::IntersectionStatus::In);
	BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Sphere(gxm::Vec3(2.0f, 0.0f, 0.0f), 3.0f)), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Sphere(gxm::Vec3(2.0f, 0.0f, 0.0f), 0.999f)), gxm::IntersectionStatus::Out);
}
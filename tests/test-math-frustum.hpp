#include <gearoenix/math/math-frustum.hpp>
#include <gearoenix/math/math-sphere.hpp>

BOOST_AUTO_TEST_CASE(math_frustum_test)
{
	namespace gxm = gearoenix::math;
	const gxm::Plane planes[6] = {
		gxm::Plane(gxm::Vec3(-1.0f, -1.0f, -1.0f), gxm::Vec3(1.0f, 0.0f, 0.0f)),
		gxm::Plane(gxm::Vec3(-1.0f, -1.0f, -1.0f), gxm::Vec3(0.0f, 1.0f, 0.0f)),
		gxm::Plane(gxm::Vec3(-1.0f, -1.0f, -1.0f), gxm::Vec3(0.0f, 0.0f, 1.0f)),
		gxm::Plane(gxm::Vec3(1.0f, 1.0f, 1.0f), gxm::Vec3(-1.0f, 0.0f, 0.0f)),
		gxm::Plane(gxm::Vec3(1.0f, 1.0f, 1.0f), gxm::Vec3(0.0f, -1.0f, 0.0f)),
		gxm::Plane(gxm::Vec3(1.0f, 1.0f, 1.0f), gxm::Vec3(0.0f, 0.0f, -1.0f)),
	};
	const gxm::Frustum f(planes);

	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(0.0f, 0.0f, 0.0f), 0.9999f)), gxm::IntersectionStatus::In);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.0f, 1.0f, 1.0f), 1.0f)), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.5f, 1.5f, 1.5f), 1.0f)), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.6f, 1.6f, 1.6f), 1.0f)), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.7f, 1.7f, 1.7f), 1.0f)), gxm::IntersectionStatus::Cut);
	BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(2.001f, 2.001f, 2.001f), 1.0f)), gxm::IntersectionStatus::Out);
}
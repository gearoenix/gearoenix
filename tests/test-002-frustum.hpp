#include <gearoenix/math/math-frustum.hpp>
#include <gearoenix/math/math-sphere.hpp>

BOOST_AUTO_TEST_CASE(math_frustum_test)
{
	namespace gxm = gearoenix::math;
	const gxm::Plane planes[6] = {
		gxm::Plane(gxm::Vec3(-1.0, -1.0, -1.0), gxm::Vec3(-1.0, 0.0, 0.0)),
		gxm::Plane(gxm::Vec3(-1.0, -1.0, -1.0), gxm::Vec3(0.0, -1.0, 0.0)),
		gxm::Plane(gxm::Vec3(-1.0, -1.0, -1.0), gxm::Vec3(0.0, 0.0, -1.0)),
		gxm::Plane(gxm::Vec3(1.0, 1.0, 1.0), gxm::Vec3(1.0, 0.0, 0.0)),
		gxm::Plane(gxm::Vec3(1.0, 1.0, 1.0), gxm::Vec3(0.0, 1.0, 0.0)),
		gxm::Plane(gxm::Vec3(1.0, 1.0, 1.0), gxm::Vec3(0.0, 0.0, 1.0)),
	};
	const gxm::Frustum f(planes);
	const gxm::Sphere s(gxm::Vec3(1.0, 1.0, 1.0), 1.0);

	BOOST_TEST(f.check_intersection(s) == gxm::IntersectionStatus::Cut);
}
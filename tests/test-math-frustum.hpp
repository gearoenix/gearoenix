#include <gearoenix/math/math-aabb.hpp>
#include <gearoenix/math/math-frustum.hpp>
#include <gearoenix/math/math-projector-frustum.hpp>
#include <gearoenix/math/math-sphere.hpp>

BOOST_AUTO_TEST_CASE(math_frustum_test)
{
    namespace gxm = gearoenix::math;

    const gxm::Mat4x4<double> identity;

    const gxm::Frustum f({
        gxm::Plane(gxm::Vec3(-1.0, -1.0, -1.0), gxm::Vec3(1.0, 0.0, 0.0)),
        gxm::Plane(gxm::Vec3(-1.0, -1.0, -1.0), gxm::Vec3(0.0, 1.0, 0.0)),
        gxm::Plane(gxm::Vec3(-1.0, -1.0, -1.0), gxm::Vec3(0.0, 0.0, 1.0)),
        gxm::Plane(gxm::Vec3(1.0, 1.0, 1.0), gxm::Vec3(-1.0, 0.0, 0.0)),
        gxm::Plane(gxm::Vec3(1.0, 1.0, 1.0), gxm::Vec3(0.0, -1.0, 0.0)),
        gxm::Plane(gxm::Vec3(1.0, 1.0, 1.0), gxm::Vec3(0.0, 0.0, -1.0)),
    });
    BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(0.0, 0.0, 0.0), 0.9999)), gxm::IntersectionStatus::In);
    BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.0, 1.0, 1.0), 1.0)), gxm::IntersectionStatus::Cut);
    BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.5, 1.5, 1.5), 1.0)), gxm::IntersectionStatus::Cut);
    BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.6, 1.6, 1.6), 1.0)), gxm::IntersectionStatus::Cut);
    BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(1.7, 1.7, 1.7), 1.0)), gxm::IntersectionStatus::Cut);
    BOOST_CHECK_EQUAL(f.check_intersection(gxm::Sphere(gxm::Vec3(2.001, 2.001, 2.001), 1.0)), gxm::IntersectionStatus::Out);

    const gxm::ProjectorFrustum pf(identity);
    BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Aabb3(gxm::Vec3(0.999, 0.999, 0.999), gxm::Vec3(-0.999, -0.999, -0.999))), gxm::IntersectionStatus::In);
    BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Aabb3(gxm::Vec3(2.0, 2.0, 2.0), gxm::Vec3(0.0, 0.0, 0.0))), gxm::IntersectionStatus::Cut);
    BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Aabb3(gxm::Vec3(2.0, 2.0, 2.0), gxm::Vec3(1.0, 1.0, 1.0))), gxm::IntersectionStatus::Out);
    BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Sphere(gxm::Vec3(0.0, 0.0, 0.0), 0.999)), gxm::IntersectionStatus::In);
    BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Sphere(gxm::Vec3(2.0, 0.0, 0.0), 3.0)), gxm::IntersectionStatus::Cut);
    BOOST_CHECK_EQUAL(pf.check_intersection(gxm::Sphere(gxm::Vec3(2.0, 0.0, 0.0), 0.999)), gxm::IntersectionStatus::Out);
}
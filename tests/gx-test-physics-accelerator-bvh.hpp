#include <gearoenix/math/math-ray.hpp>
#include <gearoenix/physics/accelerator/phs-acc-bvh.hpp>
#include <gearoenix/physics/collider/phs-cld-aabb.hpp>
#include <gearoenix/physics/collider/phs-cld-frustum.hpp>
#include <gearoenix/physics/collider/phs-cld-sphere.hpp>
#include <random>

BOOST_AUTO_TEST_CASE(physics_accelerator_bvh)
{
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<double> dis1(0.5, 0.9);
    std::uniform_real_distribution<double> dis2(-20.0, 20.0);

    gearoenix::physics::accelerator::Bvh bvh;

    std::vector<std::unique_ptr<gearoenix::physics::collider::Collider>> cs;
    std::vector<gearoenix::physics::collider::Collider*> colliders;

    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0, 0.0, 0.0), 0.9));

#define GX_TEST_HELPER                    \
    {                                     \
        colliders.clear();                \
        for (auto& c : cs)                \
            colliders.push_back(c.get()); \
    }                                     \
    bvh.reset(colliders);

    GX_TEST_HELPER

#define GX_TEST_HELPER2(x)                                               \
    {                                                                    \
        const gearoenix::math::Vec3 p(x, 0.0, 10.0);                     \
        const gearoenix::math::Vec3 d(0.0, 0.0, -1.0);                   \
        const gearoenix::math::Ray3 r(p, d);                             \
        const auto hit = bvh.hit(r, std::numeric_limits<double>::max()); \
        BOOST_TEST((*hit).first < 10.1);                                 \
        BOOST_TEST((*hit).first > 9.00);                                 \
    }

#define GX_TEST_HELPER3(x)                                               \
    {                                                                    \
        const gearoenix::math::Vec3 p(x, 0.0, 10.0);                     \
        const gearoenix::math::Vec3 d(0.0, 0.0, -1.0);                   \
        const gearoenix::math::Ray3 r(p, d);                             \
        const auto hit = bvh.hit(r, std::numeric_limits<double>::max()); \
        BOOST_TEST(!hit.has_value());                                    \
    }

    GX_TEST_HELPER2(0.0)
    GX_TEST_HELPER2(0.89)
    GX_TEST_HELPER2(-0.89)
    GX_TEST_HELPER3(1.1)
    GX_TEST_HELPER3(-1.1)

    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(2.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(1.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(-1.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(-2.0, 0.0, 0.0), 0.9));

    GX_TEST_HELPER

    GX_TEST_HELPER2(1.11)
    GX_TEST_HELPER2(2.00)
    GX_TEST_HELPER2(2.89)
    GX_TEST_HELPER2(1.0)
    GX_TEST_HELPER2(-1.0)
    GX_TEST_HELPER2(-1.11)
    GX_TEST_HELPER2(-2.0)
    GX_TEST_HELPER2(-2.89)

    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(4.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(3.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(-3.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(-4.0, 0.0, 0.0), 0.9));

    GX_TEST_HELPER

    GX_TEST_HELPER2(4.0)
    GX_TEST_HELPER2(3.0)
    GX_TEST_HELPER2(-3.0)
    GX_TEST_HELPER2(-4.0)

    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0, 0.0, 0.0), 0.9));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Sphere>(
            gearoenix::math::Vec3(0.0, 0.0, 0.0), 0.9));

    GX_TEST_HELPER

    GX_TEST_HELPER2(0.0)
    GX_TEST_HELPER2(0.0)
    GX_TEST_HELPER2(0.0)
    GX_TEST_HELPER2(0.0)

    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Aabb>(
            gearoenix::math::Vec3(7.0, 0.0, 0.9),
            gearoenix::math::Vec3(5.0, 0.0, 0.0)));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Aabb>(
            gearoenix::math::Vec3(8.0, 0.0, 0.9),
            gearoenix::math::Vec3(6.0, 0.0, 0.0)));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Aabb>(
            gearoenix::math::Vec3(9.0, 0.0, 0.9),
            gearoenix::math::Vec3(7.0, 0.0, 0.0)));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Aabb>(
            gearoenix::math::Vec3(10.0, 0.0, 0.9),
            gearoenix::math::Vec3(8.0, 0.0, 0.0)));
    cs.push_back(
        std::make_unique<gearoenix::physics::collider::Aabb>(
            gearoenix::math::Vec3(11.0, 0.0, 0.9),
            gearoenix::math::Vec3(9.0, 0.0, 0.0)));

    GX_TEST_HELPER

    GX_TEST_HELPER2(6.0)
    GX_TEST_HELPER2(7.0)
    GX_TEST_HELPER2(8.0)
    GX_TEST_HELPER2(9.0)
    GX_TEST_HELPER2(10.0)

    GX_TEST_HELPER3(12.0)
    GX_TEST_HELPER3(13.0)
    GX_TEST_HELPER3(14.0)
    GX_TEST_HELPER3(15.0)
    GX_TEST_HELPER3(-7.0)
    GX_TEST_HELPER3(-8.0)

    for (int i = 0; i < 2000; ++i) {
        cs.push_back(
            std::make_unique<gearoenix::physics::collider::Sphere>(
                gearoenix::math::Vec3(dis2(gen), dis2(gen), dis2(gen)), dis1(gen)));
    }

    GX_TEST_HELPER
    auto now1 = std::chrono::high_resolution_clock::now();
    bvh.reset(colliders);
    auto now2 = std::chrono::high_resolution_clock::now();
    GXLOGD("dis-time: " << (now2 - now1).count())

    long double dis_time = 0.0L;

    constexpr int hits_count = 100000;

    for (int i = 0; i < hits_count; ++i) {
        const gearoenix::math::Vec3 p(dis2(gen), dis2(gen), dis2(gen));
        const gearoenix::math::Vec3 t(dis2(gen), dis2(gen), dis2(gen));
        const gearoenix::math::Ray3 r(p, t.normalized());
        now1 = std::chrono::high_resolution_clock::now();
        (void)bvh.hit(r, std::numeric_limits<double>::max());
        now2 = std::chrono::high_resolution_clock::now();
        dis_time += (now2 - now1).count();
    }
    GXLOGD("dis-time for " << hits_count << " hits: " << dis_time)

    //    cs.clear();
    //    cs.push_back(
    //            std::make_unique<gearoenix::physics::collider::Aabb>(
    //                    gearoenix::math::Vec3(-11.0, 0.0, 0.0) + 17.1,
    //                    gearoenix::math::Vec3(-11.0, 0.0, 0.0) - 17.1));
    //
    //    GX_TEST_HELPER
    //    gearoenix::physics::collider::Frustum frustum;
    //    frustum.set_view_projection(
    //            gearoenix::math::Mat4x4<double>::perspective(2.0, 2.0, 1.0, 10) *
    //                    gearoenix::math::Mat4x4<double>::look_at(
    //                            gearoenix::math::Vec3(1.75, 0.0, 0.0),
    //                            gearoenix::math::Vec3(-11.0, 0.0, 0.0),
    //                            gearoenix::math::Vec3(0.0, 0.0, 1.0)));
    //    int intersections_count = 0;
    //    bvh.call_on_intersecting(&frustum, [&](gearoenix::physics::collider::Collider* const) noexcept {
    //        ++intersections_count;
    //    });
    //
    //    BOOST_TEST(intersections_count > 0);
}
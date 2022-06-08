#ifndef GEAROENIX_PHYSICS_ACCELERATOR_BVH_TEST_HPP
#define GEAROENIX_PHYSICS_ACCELERATOR_BVH_TEST_HPP
#include "../../math/gx-math-frustum.hpp"
#include "gx-phs-acc-bvh.hpp"

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh)
{
    struct Data final {
        int id = 0;
    };
    using Box = gearoenix::math::Aabb3<double>;
    using V2 = gearoenix::math::Vec2<double>;
    using V3 = gearoenix::math::Vec3<double>;
    using Frustum = gearoenix::math::Frustum<double>;

    gearoenix::physics::accelerator::Bvh<Data> bvh;

    bvh.add({ Box(V3(1.0), V3(-1.0)), Data { 0 } });

    bvh.create_nodes();

    bvh.reset();

    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 11; ++j) {
            const V3 p(V2(static_cast<double>(i), static_cast<double>(j)) * 3.0 - 15.0, 0.0);
            bvh.add({ Box(p + 1.0, p - 1.0), Data { (i << 8) | j } });
        }
    }

    bvh.create_nodes();

    Frustum frustum(std::array<V3, 8> {
        V3(-15, 15.0, -1.0), V3(15, 15.0, -1.0), V3(-15, -15.0, -1.0), V3(15.0, -15.0, -1.0), // 0 --------
        V3(-15, 15.0, 1.0), V3(15, 15.0, 1.0), V3(-15, -15.0, 1.0), V3(15.0, -15.0, 1.0) // 1 -------------
    });

    BOOST_TEST(frustum.check_intersection(Box(V3(1.0, 1.0, 0.5), V3(-1.0, -1.0, -0.5))));
    BOOST_TEST(frustum.check_intersection(Box(V3(20.0, 20.0, 2.5), V3(-20.0, -20.0, -2.5))));

    int count = 0;
    bvh.call_on_intersecting(frustum, [&](auto&) { ++count; });
    BOOST_TEST(count == 11 * 11);

    frustum = Frustum(std::array<V3, 8> {
        V3(-10, 10.0, -0.5), V3(10, 10.0, -0.5), V3(-10.0, -10.0, -0.5), V3(10.0, -10.0, -0.5), // 0 --------
        V3(-10, 10.0, 0.5), V3(10, 10.0, 0.5), V3(-10.0, -10.0, 0.5), V3(10.0, -10.0, 0.5) // 1 -------------
    });

    BOOST_TEST(!frustum.check_intersection(Box(V3(-11.0, -11.0, 1.0), V3(-13.0, -13.0, -1.0))));

    count = 0;
    bvh.call_on_intersecting(frustum, [&](auto& d) {
        ++count;
        const int i = d.user_data.id & 255;
        const int j = (d.user_data.id >> 8) & 255;
        const bool b = i > 1 && i < 9 && j > 1 && j < 9;
        BOOST_TEST(b);
    });
    BOOST_TEST(count == 7 * 7);
}

#endif
#ifndef GEAROENIX_PHYSICS_ACCELERATOR_BVH_TEST_HPP
#define GEAROENIX_PHYSICS_ACCELERATOR_BVH_TEST_HPP
#include "gx-phs-acc-bvh.hpp"

BOOST_AUTO_TEST_CASE(gearoenix_accelerator_bvh)
{
    struct Data final {
    };
    using Box = gearoenix::math::Aabb3<double>;
    using V3 = gearoenix::math::Vec3<double>;

    gearoenix::physics::accelerator::Bvh<Data> bvh;

    bvh.add({ Box(V3(1.0), V3(-1.0)), Data {} });

    bvh.create_nodes();

    // BOOST_TEST(nullptr == a5);
}

#endif
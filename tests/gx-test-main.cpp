#define BOOST_TEST_MODULE "Tests for Gearoenix game engine"
#include <boost/test/included/unit_test.hpp>

#define GX_TEST_FLOAT_NEAR(x, y)                                                                                  \
    if (x != y && std::abs(x) + std::abs(y) > 0.0001 && std::abs(x - y) / (std::abs(x) + std::abs(y)) > 0.0001) { \
        BOOST_TEST_CHECK(x == y);                                                                                 \
    }                                                                                                             \
    static_assert(true, "")

#include <gearoenix/core/allocator/gx-cr-alc-range-test.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-world-test.hpp>
#include <gearoenix/core/sync/gx-cr-sync-parallel-for-test.hpp>
#include <gearoenix/math/gx-math-matrix-3d-test.hpp>
#include <gearoenix/math/gx-math-matrix-4d-test.hpp>
#include <gearoenix/math/gx-math-quaternion-test.hpp>
#include <gearoenix/net/gx-net-manager-test.hpp>
#include <gearoenix/physics/accelerator/gx-phs-acc-bvh-test.hpp>
#include <gearoenix/physics/gx-phs-transformation-test.hpp>

struct GlobalFixture final {
    gearoenix::net::Manager net_manager;

    GlobalFixture()
    {
        gearoenix::core::job::initialise();
    }

    ~GlobalFixture()
    {
        gearoenix::core::job::terminate();
    }
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalFixture);
#define BOOST_TEST_MODULE "Tests for Gearoenix game engine"
#include <boost/test/included/unit_test.hpp>
// Making types printable for boost
//#include <gearoenix/math/gx-math-intersection-status.hpp>

//#define GX_TEST_PRINT_TYPE_VAL(x)                                                        \
//    namespace boost::test_tools::tt_detail {                                             \
//    template <>                                                                          \
//    struct print_log_value<gearoenix::x> {                                               \
//        void operator()(std::ostream& os, const gearoenix::x v) { ::operator<<(os, v); } \
//    };                                                                                   \
//    }
// GX_TEST_PRINT_TYPE_VAL(math::IntersectionStatus)
//  Test units

#include <gearoenix/core/ecs/gx-cr-ecs-world-test.hpp>
#include <gearoenix/core/gx-cr-allocator-test.hpp>
#include <gearoenix/math/gx-math-matrix-4d-test.hpp>
#include <gearoenix/physics/accelerator/gx-phs-acc-bvh-test.hpp>
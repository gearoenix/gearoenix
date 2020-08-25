#define BOOST_TEST_MODULE "Tests for Gearoenix game engine"
#include <boost/test/included/unit_test.hpp>
// Making types printable for boost
#include <gearoenix/math/math-intersection-status.hpp>
#define GX_TEST_PRINT_TYPE_VAL(x)                                                        \
    namespace boost::test_tools::tt_detail {                                             \
    template <>                                                                          \
    struct print_log_value<gearoenix::x> {                                               \
        void operator()(std::ostream& os, const gearoenix::x v) { ::operator<<(os, v); } \
    };                                                                                   \
    }
GX_TEST_PRINT_TYPE_VAL(math::IntersectionStatus)
// Test units
#include "gx-test-ai-dijkstra.hpp"
#include "gx-test-ai-graph.hpp"
#include "gx-test-core-pool.hpp"
#include "gx-test-math-frustum.hpp"
#include "gx-test-math-vector.hpp"
#include "gx-test-physics-accelerator-bvh.hpp"
#include "gx-test-render-texture.hpp"
#include <gearoenix/core/sync/gx-cr-sync-influence-manager-test.hpp>
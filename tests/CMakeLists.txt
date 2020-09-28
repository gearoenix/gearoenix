SET (GX_TEST_NEEDS)

INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR} ${GX_BOOST_INCLUDE_DIRS} ${GX_GLM_INCLUDE_DIRS})

ADD_EXECUTABLE (GearoenixTests
        gx-test-ai-dijkstra.hpp
        gx-test-ai-graph.hpp
        gx-test-cr-allocator.hpp
        gx-test-cr-pool.hpp
        gx-test-cr-sync-influence-manager.hpp
        gx-test-math-frustum.hpp
        gx-test-math-vector.hpp
        gx-test-phs-acc-bvh.hpp
        gx-test-rnd-texture.hpp

        gx-test-main.cpp)
TARGET_LINK_LIBRARIES (GearoenixTests Gearoenix ${GX_TEST_NEEDS})
ADD_TEST (NAME GearoenixTests COMMAND GearoenixTests)
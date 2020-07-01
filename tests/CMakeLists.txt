SET (GX_TEST_NEEDS)

INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR} ${GX_BOOST_INCLUDE_DIRS} ${GX_GLM_INCLUDE_DIRS})

ADD_EXECUTABLE (GearoenixTests
    gx-test-ai-dijkstra.hpp
    gx-test-ai-graph.hpp
    gx-test-core-pool.hpp
    gx-test-math-vector.hpp
    gx-test-math-frustum.hpp
    gx-test-physics-accelerator-bvh.hpp
    gx-test-render-texture.hpp
    gx-test-main.cpp)
TARGET_LINK_LIBRARIES (GearoenixTests Gearoenix ${GX_TEST_NEEDS})
ADD_TEST (NAME GearoenixTests COMMAND GearoenixTests)
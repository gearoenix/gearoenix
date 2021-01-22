INCLUDE(CTest)
ENABLE_TESTING()

ADD_EXECUTABLE (GearoenixGameEngineTests
        ${GX_ENGINE_TEST_SRC}
#        gx-test-ai-dijkstra.hpp
#        gx-test-ai-graph.hpp
#        gx-test-cr-allocator.hpp
#        gx-test-cr-pool.hpp
#        gx-test-cr-sync-influence-manager.hpp
#        gx-test-math-frustum.hpp
#        gx-test-math-vector.hpp
#        gx-test-phs-acc-bvh.hpp
#        gx-test-rnd-texture.hpp
        gx-test-main.cpp)

TARGET_LINK_LIBRARIES (GearoenixGameEngineTests GearoenixGameEngine)
TARGET_INCLUDE_DIRECTORIES (GearoenixGameEngineTests PRIVATE ${CMAKE_SOURCE_DIR})
ADD_TEST (NAME GearoenixGameEngineTests COMMAND GearoenixGameEngineTests)
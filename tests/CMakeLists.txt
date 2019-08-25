SET (GX_TEST_NEEDS)

INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR} ${GX_BOOST_INCLUDE_DIRS} ${GX_GLM_INCLUDE_DIRS})

ADD_EXECUTABLE (GearoenixTests 
    test-core-pool.hpp
    test-math-vector.hpp
    test-math-frustum.hpp
    test-main.cpp)
TARGET_LINK_LIBRARIES (GearoenixTests Gearoenix ${GX_TEST_NEEDS})
ADD_TEST (NAME GearoenixTests COMMAND GearoenixTests)
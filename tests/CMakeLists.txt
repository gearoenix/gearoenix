INCLUDE(CTest)
ENABLE_TESTING()

ADD_EXECUTABLE (GearoenixGameEngineTests
        ${GX_ENGINE_TEST_SRC}
        gx-test-main.cpp)

TARGET_LINK_LIBRARIES (GearoenixGameEngineTests GearoenixGameEngine)
TARGET_INCLUDE_DIRECTORIES (GearoenixGameEngineTests PRIVATE ${CMAKE_SOURCE_DIR} ${GX_GLM_PATH})
ADD_TEST (NAME GearoenixGameEngineTests COMMAND GearoenixGameEngineTests)
MESSAGE(STATUS "Gearoenix test target has been  created.")
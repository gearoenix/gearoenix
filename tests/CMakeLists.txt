include(CTest)
enable_testing()

gx_prepare_target(GearoenixGameEngineTests SOURCES ${GX_ENGINE_TEST_SRC} gx-test-main.cpp)
target_include_directories(GearoenixGameEngineTests PRIVATE ${CMAKE_SOURCE_DIR} ${GX_GLM_PATH})
add_test(NAME GearoenixGameEngineTests COMMAND GearoenixGameEngineTests)
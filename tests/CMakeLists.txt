include(CTest)
enable_testing()

gx_prepare_target(GearoenixGameEngineTests SOURCES ${GX_ENGINE_TEST_SRC} gx-test-main.cpp)
target_include_directories(GearoenixGameEngineTests PRIVATE ${CMAKE_SOURCE_DIR} ${GX_SUBMODULES_DIR}/glm)
target_link_libraries(GearoenixGameEngineTests PRIVATE Boost::unit_test_framework)
target_compile_definitions(GearoenixGameEngineTests PRIVATE GLM_ENABLE_EXPERIMENTAL)
add_test(NAME GearoenixGameEngineTests COMMAND GearoenixGameEngineTests)
set(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/007-reflection-probe)
set(GX_SRC ${GX_SRC_DIR}/gx-ex-007-main.cpp)

if (GX_ENABLE_EXAMPLE_007_REFLECTION_PROBE OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixExample007ReflectionProbe SOURCES ${GX_SRC})
endif ()
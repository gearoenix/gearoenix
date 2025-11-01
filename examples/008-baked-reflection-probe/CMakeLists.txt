set(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/008-baked-reflection-probe)
set(GX_SRC ${GX_SRC_DIR}/gx-ex-008-main.cpp)

if (GX_ENABLE_EXAMPLE_008_BAKED_REFLECTION_PROBE OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixExample008BakedReflectionProbe SOURCES ${GX_SRC})
endif ()
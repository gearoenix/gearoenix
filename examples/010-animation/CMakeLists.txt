set(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/010-animation)
set(GX_SRC ${GX_SRC_DIR}/gx-ex-010-main.cpp)

if (GX_ENABLE_EXAMPLE_010_ANIMATION OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixExample010Animation SOURCES ${GX_SRC})
endif ()
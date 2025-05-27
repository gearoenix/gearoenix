set(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/011-animation)
set(GX_SRC ${GX_SRC_DIR}/gx-ex-011-main.cpp)

if (GX_ENABLE_EXAMPLE_011_ANIMATION OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixExample011Animation SOURCES ${GX_SRC})
endif ()
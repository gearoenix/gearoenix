INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/007-hello-skybox)

SET(GX_SRC ${GX_SRC_DIR}/gx-ex-007-main.cpp)

IF (NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    ADD_EXECUTABLE(GearoenixExample007HelloSkybox ${GEAROENIX_EXE_TARGET} ${GX_SRC})
    GX_ENGINE_PREPARE_TARGET(GearoenixExample007HelloSkybox)
ELSEIF (GX_ENABLE_EXAMPLE_007_HELLO_SKYBOX)
    ADD_LIBRARY(main SHARED ${GX_SRC})
    GX_ENGINE_PREPARE_TARGET(main)
ENDIF ()
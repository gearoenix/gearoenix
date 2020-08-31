INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(GX_DEMO_WORLD_WIDE_RACING_SRC
        gx-demo-wwr-car.cpp
        gx-demo-wwr-car.hpp
        gx-demo-wwr-count-down.cpp
        gx-demo-wwr-count-down.hpp
        gx-demo-wwr-garage.cpp
        gx-demo-wwr-garage.hpp
        gx-demo-wwr-main.cpp
        gx-demo-wwr-main.hpp
        gx-demo-wwr-rotating-button.cpp
        gx-demo-wwr-rotating-button.hpp
        gx-demo-wwr-terrain-manager.cpp
        gx-demo-wwr-terrain-manager.hpp
        gx-demo-wwr-training.cpp
        gx-demo-wwr-training.hpp)

IF (NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    ADD_EXECUTABLE(GearoenixDemoWorldWideRacing ${GX_DEMO_WORLD_WIDE_RACING_SRC})
    TARGET_LINK_LIBRARIES(GearoenixDemoWorldWideRacing Gearoenix)
    MESSAGE(STATUS "World wide racing demo has been configured.")
ELSE ()
    ADD_LIBRARY(main SHARED ${GX_DEMO_WORLD_WIDE_RACING_SRC})
    TARGET_LINK_LIBRARIES(main Gearoenix)
ENDIF ()
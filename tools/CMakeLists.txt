INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(GX_TOOL_IBL_BAKER_SRC
        ibl-baker/gx-tool-ibl-main.cpp
        ibl-baker/gx-tool-ibl-main.hpp)

IF (NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    ADD_EXECUTABLE(GearoenixToolIBLBaker ${GX_TOOL_IBL_BAKER_SRC})
    TARGET_LINK_LIBRARIES(GearoenixToolIBLBaker Gearoenix)
ELSE ()
    ADD_LIBRARY(main SHARED ${GX_TOOL_IBL_BAKER_SRC})
    TARGET_LINK_LIBRARIES(main Gearoenix)
ENDIF ()
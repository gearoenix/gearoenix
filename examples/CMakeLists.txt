INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(GX_EXAMPLE_001_HELLO_WORLD_SRC
        001-hello-world/exm-001-hello-world.cpp
        001-hello-world/exm-001-hello-world.hpp)

SET(GX_EXAMPLE_002_ASSET_LOADING_SRC
        002-asset-loading/exm-002-asset-loading.cpp
        002-asset-loading/exm-002-asset-loading.hpp)

SET(GX_EXAMPLE_003_NET_HELLO_WORLD_SRC
        003-net-hello-world/exm-003-net-hello-world.cpp
        003-net-hello-world/exm-003-net-hello-world.hpp)

IF (NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    ADD_EXECUTABLE(GearoenixExample001HelloWorld ${GX_EXAMPLE_001_HELLO_WORLD_SRC})
    TARGET_LINK_LIBRARIES(GearoenixExample001HelloWorld Gearoenix)
    ADD_EXECUTABLE(GearoenixExample002AssetLoading ${GX_EXAMPLE_002_ASSET_LOADING_SRC})
    TARGET_LINK_LIBRARIES(GearoenixExample002AssetLoading Gearoenix)
    ADD_EXECUTABLE(GearoenixExample003NetHelloWorld ${GX_EXAMPLE_003_NET_HELLO_WORLD_SRC})
    TARGET_LINK_LIBRARIES(GearoenixExample003NetHelloWorld Gearoenix)
ELSEIF (GX_ENABLE_EXAMPLE_001_HELLO_WORLD)
    ADD_LIBRARY(main SHARED ${GX_EXAMPLE_001_HELLO_WORLD_SRC})
ELSEIF (GX_ENABLE_EXAMPLE_002_ASSET_LOADING)
    ADD_LIBRARY(main SHARED ${GX_EXAMPLE_002_ASSET_LOADING_SRC})
ELSEIF (GX_ENABLE_EXAMPLE_003_NET_HELLO_WORLD)
    ADD_LIBRARY(main SHARED ${GX_EXAMPLE_003_NET_HELLO_WORLD_SRC})
ENDIF ()

IF (CMAKE_SYSTEM_NAME MATCHES "Android")
    TARGET_LINK_LIBRARIES(main Gearoenix)
ENDIF ()
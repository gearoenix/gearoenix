INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(GX_EXAMPLE_001_HELLO_WORLD_SRC
        001-hello-world/exm-001-hello-world.cpp
        001-hello-world/exm-001-hello-world.hpp)

SET(GX_EXAMPLE_002_ASSET_LOADING_SRC
        002-asset-loading/exm-002-asset-loading.cpp
        002-asset-loading/exm-002-asset-loading.hpp)

SET(GX_EXAMPLE_003_UI_WIDGETS_SRC
        003-ui-widgets/exm-003-ui-widgets.cpp
        003-ui-widgets/exm-003-ui-widgets.hpp)

IF (NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    ADD_EXECUTABLE(GearoenixExample001HelloWorld ${GX_EXAMPLE_001_HELLO_WORLD_SRC})
    TARGET_LINK_LIBRARIES(GearoenixExample001HelloWorld Gearoenix)
    ADD_EXECUTABLE(GearoenixExample002AssetLoading ${GX_EXAMPLE_002_ASSET_LOADING_SRC})
    TARGET_LINK_LIBRARIES(GearoenixExample002AssetLoading Gearoenix)
    ADD_EXECUTABLE(GearoenixExample003UiWidgets ${GX_EXAMPLE_003_UI_WIDGETS_SRC})
    TARGET_LINK_LIBRARIES(GearoenixExample003UiWidgets Gearoenix)
ELSEIF (GX_ENABLE_EXAMPLE_001_HELLO_WORLD)
    ADD_LIBRARY(main SHARED ${GX_EXAMPLE_001_HELLO_WORLD_SRC})
ELSEIF (GX_ENABLE_EXAMPLE_002_ASSET_LOADING)
    ADD_LIBRARY(main SHARED ${GX_EXAMPLE_002_ASSET_LOADING_SRC})
ELSEIF (GX_ENABLE_EXAMPLE_003_UI_WIDGETS)
    ADD_LIBRARY(main SHARED ${GX_EXAMPLE_003_UI_WIDGETS_SRC})
ENDIF ()

IF (CMAKE_SYSTEM_NAME MATCHES "Android")
    TARGET_LINK_LIBRARIES(main Gearoenix)
ENDIF ()
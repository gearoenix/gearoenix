INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(GX_EDITOR_SRC_DIR ${CMAKE_SOURCE_DIR}/editor)

ADD_SUBDIRECTORY(ui)
ADD_SUBDIRECTORY(viewport)

SET(GX_EDITOR_SRC
        ${GX_EDITOR_UI_SRC}
        ${GX_EDITOR_VIEWPORT_SRC}
        gx-editor-main.cpp
        gx-editor-main.hpp)

SOURCE_GROUP(TREE ${GX_EDITOR_SRC_DIR} FILES ${GX_EDITOR_SRC})

IF (NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    ADD_EXECUTABLE(GearoenixEditor ${GEAROENIX_EXE_TARGET} ${GX_EDITOR_SRC})
    GX_ENGINE_PREPARE_TARGET(GearoenixEditor)
ELSEIF (GX_ENABLE_EDITOR)
    ADD_LIBRARY(main SHARED ${GX_EDITOR_SRC})
    GX_ENGINE_PREPARE_TARGET(main)
ENDIF ()
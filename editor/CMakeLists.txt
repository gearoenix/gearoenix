set(GX_EDITOR_SRC_DIR ${CMAKE_SOURCE_DIR}/editor)

add_subdirectory(ui)
add_subdirectory(viewport)

set(GX_EDITOR_SRC ${GX_EDITOR_UI_SRC} ${GX_EDITOR_VIEWPORT_SRC} gx-editor-main.cpp gx-editor-main.hpp)

source_group(TREE ${GX_EDITOR_SRC_DIR} FILES ${GX_EDITOR_SRC})

if (GX_ENABLE_EDITOR OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixEditor SOURCES ${GX_EDITOR_SRC})
endif ()
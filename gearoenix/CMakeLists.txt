set(GX_ENGINE_SRC_DIR ${CMAKE_SOURCE_DIR}/gearoenix)

message(STATUS "Gearoenix game engine source directory is ${GX_ENGINE_SRC_DIR}.")

file(GLOB_RECURSE GX_ENGINE_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp")

set(GX_ENGINE_TEST_SRC ${GX_ENGINE_SRC})
list(FILTER GX_ENGINE_SRC EXCLUDE REGEX ".*-test\\.hpp$")
list(FILTER GX_ENGINE_TEST_SRC INCLUDE REGEX ".*-test\\.hpp$")

source_group(TREE ${GX_ENGINE_SRC_DIR} FILES ${GX_ENGINE_SRC})
source_group(TREE ${GX_ENGINE_SRC_DIR} FILES ${GX_ENGINE_TEST_SRC})

set(GX_ENGINE_TEST_SRC ${GX_ENGINE_TEST_SRC} PARENT_SCOPE)

set(GX_IMGUI_PATH ${GX_SUBMODULES_DIR}/ImGui)
add_library(ImGui STATIC
        ${GX_IMGUI_PATH}/imgui.cpp
        ${GX_IMGUI_PATH}/imgui_demo.cpp
        ${GX_IMGUI_PATH}/imgui_draw.cpp
        ${GX_IMGUI_PATH}/imgui_tables.cpp
        ${GX_IMGUI_PATH}/imgui_widgets.cpp
        ${GX_IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
        ${GX_IMGUI_PATH}/backends/imgui_impl_vulkan.cpp
        ${GX_IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp)
target_compile_options(ImGui PRIVATE ${GX_NO_WARNING_FLAG})
target_compile_definitions(ImGui PRIVATE IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
target_include_directories(ImGui PRIVATE ${GX_IMGUI_PATH})
target_link_libraries(ImGui PUBLIC Vulkan::Headers)

set(GX_IMGUI_FB_PATH ${GX_SUBMODULES_DIR}/ImGuiFileDialog)
add_library(ImGuiFileDialog STATIC ${GX_IMGUI_FB_PATH}/ImGuiFileDialog.cpp)
target_compile_options(ImGuiFileDialog PRIVATE ${GX_NO_WARNING_FLAG})
target_include_directories(ImGuiFileDialog PRIVATE ${GX_IMGUI_PATH})

set(GX_IMGIZMO_PATH ${GX_SUBMODULES_DIR}/ImGuizmo)
add_library(ImGuizmo STATIC
        ${GX_IMGIZMO_PATH}/GraphEditor.cpp
        ${GX_IMGIZMO_PATH}/ImCurveEdit.cpp
        ${GX_IMGIZMO_PATH}/ImGradient.cpp
        ${GX_IMGIZMO_PATH}/ImGuizmo.cpp
        ${GX_IMGIZMO_PATH}/ImSequencer.cpp)
target_compile_options(ImGuizmo PRIVATE ${GX_NO_WARNING_FLAG})
target_include_directories(ImGuizmo PRIVATE ${GX_IMGUI_PATH})

add_library(GearoenixGameEngine STATIC ${GX_ENGINE_SRC})
target_link_libraries(GearoenixGameEngine PUBLIC ImGui ImGuiFileDialog ImGuizmo SDL2::SDL2 Boost::headers)
target_include_directories(GearoenixGameEngine PUBLIC ${GX_IMGUI_PATH})
target_include_directories(GearoenixGameEngine PUBLIC ${GX_SUBMODULES_DIR})

foreach (GX_LIB ${GX_DEBUG_LIBS})
    target_link_libraries(GearoenixGameEngine PUBLIC debug ${GX_LIB})
endforeach ()

foreach (GX_LIB ${GX_RELEASE_LIBS})
    target_link_libraries(GearoenixGameEngine PUBLIC optimized ${GX_LIB})
endforeach ()

add_custom_target(GearoenixAssets)

function(gx_prepare_target GX_CURRENT_TARGET)
    message(STATUS "Preparing target ${GX_CURRENT_TARGET}")

    cmake_parse_arguments(PARSED_ARGS "CUSTOM_MAIN" "" "SOURCES" ${ARGN})

    if (NOT PARSED_ARGS_SOURCES)
        message(FATAL_ERROR "gx_prepare_target: SOURCES argument is required.")
    endif (NOT PARSED_ARGS_SOURCES)

    if (${GX_IN_ANDROID})
        add_library(main SHARED ${PARSED_ARGS_SOURCES})
        set(GX_CURRENT_TARGET main)
    else ()
        add_executable(${GX_CURRENT_TARGET} ${GEAROENIX_EXE_TARGET} ${PARSED_ARGS_SOURCES})
    endif ()
    target_include_directories(${GX_CURRENT_TARGET} PRIVATE ${CMAKE_SOURCE_DIR})

    if (NOT PARSED_ARGS_CUSTOM_MAIN)
        target_link_libraries(${GX_CURRENT_TARGET} PUBLIC SDL2::SDL2main)
    endif (NOT PARSED_ARGS_CUSTOM_MAIN)

    if (APPLE)
        gx_metal_prepare_target(${GX_CURRENT_TARGET})
        target_link_libraries(${GX_CURRENT_TARGET} "-framework Metal -framework MetalKit")
        if (GX_IN_IOS)
            target_link_libraries(${GX_CURRENT_TARGET} "-framework UIKit")
        else (GX_IN_IOS)
            target_link_libraries(${GX_CURRENT_TARGET} "-framework Cocoa")
        endif (GX_IN_IOS)
    endif (APPLE)
    target_link_libraries(${GX_CURRENT_TARGET} PRIVATE GearoenixGameEngine)

    if (GX_IN_ANDROID)
        set(GX_ASSET_DIR ${CMAKE_SOURCE_DIR}/android/app/src/main/assets)
    else ()
        set(GX_ASSET_DIR $<TARGET_FILE_DIR:${GX_CURRENT_TARGET}>/assets)
    endif ()
    # In WASM, for now, we have different approach for tha assets.
    if (NOT GX_IN_WASM)
        add_custom_command(
                TARGET ${GX_CURRENT_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${GX_ASSET_DIR})
        file(GLOB GX_ASSET_FILES ${GX_MAIN_ASSETS_PATH}/*)
        foreach (GX_ASSET_FILE ${GX_ASSET_FILES})
            add_custom_command(
                    TARGET ${GX_CURRENT_TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GX_ASSET_FILE} ${GX_ASSET_DIR})
        endforeach ()
        add_dependencies(${GX_CURRENT_TARGET} GearoenixAssets)
    endif ()
endfunction(gx_prepare_target)

set(GX_ENGINE_SRC_DIR ${GX_ENGINE_SRC_DIR} PARENT_SCOPE)
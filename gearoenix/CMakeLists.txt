set(GX_ENGINE_SRC_DIR ${CMAKE_SOURCE_DIR}/gearoenix)

message(STATUS "Gearoenix game engine source directory is ${GX_ENGINE_SRC_DIR}.")

file(GLOB_RECURSE GX_ENGINE_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp" "*.mm")

set(GX_ENGINE_TEST_SRC ${GX_ENGINE_SRC})
list(FILTER GX_ENGINE_SRC EXCLUDE REGEX ".*-test\\.hpp$")
list(FILTER GX_ENGINE_TEST_SRC INCLUDE REGEX ".*-test\\.hpp$")

source_group(TREE ${GX_ENGINE_SRC_DIR} FILES ${GX_ENGINE_SRC})
source_group(TREE ${GX_ENGINE_SRC_DIR} FILES ${GX_ENGINE_TEST_SRC})

set(GX_ENGINE_TEST_SRC ${GX_ENGINE_TEST_SRC} PARENT_SCOPE)

function(gx_common_target_config GX_CURRENT_TARGET)
    if (GX_IN_WASM)
        target_compile_options(${GX_CURRENT_TARGET} PUBLIC
                -pthread
                $<$<CONFIG:Release>:-O3>
                $<$<CONFIG:Release>:-ffast-math>
                $<$<CONFIG:Release>:-flto=full>
                $<$<CONFIG:Debug>:-fdebug-compilation-dir=${CMAKE_SOURCE_DIR}>)
        target_link_options(${GX_CURRENT_TARGET} PUBLIC
                -pthread
                -fwasm-exceptions
                -sNO_EXIT_RUNTIME=1
                -sALLOW_MEMORY_GROWTH=1
                -sPTHREAD_POOL_SIZE="navigator.hardwareConcurrency*4"
                -sMIN_WEBGL_VERSION=2
                -sMAX_WEBGL_VERSION=2
                -sALLOW_TABLE_GROWTH=1
                -sEXPORTED_RUNTIME_METHODS=[ccall,HEAPU8]
                -sEXPORTED_FUNCTIONS=[_main,_malloc,_free,_gearoenix_upload_file_return]
                --preload-file=${CMAKE_SOURCE_DIR}/assets@/assets/
                --shell-file=${CMAKE_SOURCE_DIR}/wasm/template.html
                $<$<CONFIG:Release>:-O3>
                $<$<CONFIG:Release>:-ffast-math>
                $<$<CONFIG:Release>:-flto=full>
                $<$<CONFIG:Debug>:-sASSERTIONS=1>
                $<$<CONFIG:Debug>:-sGL_ASSERTIONS=1>
                $<$<CONFIG:Debug>:-sSAFE_HEAP=1>
                $<$<CONFIG:Debug>:-fdebug-compilation-dir=${CMAKE_SOURCE_DIR}>)
    elseif (GX_IN_APPLE)
        target_compile_options(${GX_CURRENT_TARGET} PUBLIC -xobjective-c++)
    endif (GX_IN_WASM)
endfunction(gx_common_target_config)

set(GX_IMGUI_PATH ${GX_SUBMODULES_DIR}/ImGui)
add_library(ImGui STATIC
        ${GX_IMGUI_PATH}/imgui.cpp
        ${GX_IMGUI_PATH}/imgui_demo.cpp
        ${GX_IMGUI_PATH}/imgui_draw.cpp
        ${GX_IMGUI_PATH}/imgui_tables.cpp
        ${GX_IMGUI_PATH}/imgui_widgets.cpp
        ${GX_IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
        ${GX_IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp)
target_compile_options(ImGui PRIVATE ${GX_NO_WARNING_FLAG})
target_include_directories(ImGui PRIVATE ${GX_IMGUI_PATH})
gx_common_target_config(ImGui)
if(NOT GX_IN_WASM)
    target_sources(ImGui PRIVATE ${GX_IMGUI_PATH}/backends/imgui_impl_vulkan.cpp)
    target_compile_definitions(ImGui PRIVATE IMGUI_IMPL_VULKAN_NO_PROTOTYPES)
    target_link_libraries(ImGui PUBLIC Vulkan::Headers)
endif (NOT GX_IN_WASM)

set(GX_IMGUI_FB_PATH ${GX_SUBMODULES_DIR}/ImGuiFileDialog)
add_library(ImGuiFileDialog STATIC ${GX_IMGUI_FB_PATH}/ImGuiFileDialog.cpp)
target_compile_options(ImGuiFileDialog PRIVATE ${GX_NO_WARNING_FLAG})
target_include_directories(ImGuiFileDialog PRIVATE ${GX_IMGUI_PATH})
gx_common_target_config(ImGuiFileDialog)

set(GX_IMGIZMO_PATH ${GX_SUBMODULES_DIR}/ImGuizmo)
add_library(ImGuizmo STATIC
        ${GX_IMGIZMO_PATH}/GraphEditor.cpp
        ${GX_IMGIZMO_PATH}/ImCurveEdit.cpp
        ${GX_IMGIZMO_PATH}/ImGradient.cpp
        ${GX_IMGIZMO_PATH}/ImGuizmo.cpp
        ${GX_IMGIZMO_PATH}/ImSequencer.cpp)
target_compile_options(ImGuizmo PRIVATE ${GX_NO_WARNING_FLAG})
target_include_directories(ImGuizmo PRIVATE ${GX_IMGUI_PATH})
gx_common_target_config(ImGuizmo)

add_library(GearoenixGameEngine STATIC ${GX_ENGINE_SRC})
target_link_libraries(GearoenixGameEngine PUBLIC ImGui ImGuiFileDialog ImGuizmo SDL3::SDL3 Boost::headers)
target_include_directories(GearoenixGameEngine PUBLIC ${GX_IMGUI_PATH})
target_include_directories(GearoenixGameEngine PUBLIC ${GX_SUBMODULES_DIR})
gx_common_target_config(GearoenixGameEngine)

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

    target_link_libraries(${GX_CURRENT_TARGET} PRIVATE GearoenixGameEngine)

    if (GX_IN_ANDROID)
        set(GX_ASSET_DIR ${CMAKE_SOURCE_DIR}/android/app/src/main/assets)
        add_custom_command(TARGET ${GX_CURRENT_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E rm -rf "${GX_ASSET_DIR}"
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${GX_MAIN_ASSETS_PATH}" "${GX_ASSET_DIR}"
                VERBATIM)
    else ()
        set(GX_ASSET_DIR $<TARGET_FILE_DIR:${GX_CURRENT_TARGET}>/assets)
        add_custom_command(TARGET ${GX_CURRENT_TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E rm -rf "${GX_ASSET_DIR}" VERBATIM)
    endif ()

    add_custom_command(TARGET ${GX_CURRENT_TARGET} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${GX_MAIN_ASSETS_PATH}" VERBATIM)

    if (GX_IN_WASM)
        set_target_properties(${GX_CURRENT_TARGET} PROPERTIES SUFFIX ".html")
    elseif (WIN32)
        add_custom_command(TARGET ${GX_CURRENT_TARGET} POST_BUILD
                COMMAND powershell.exe -NoProfile -ExecutionPolicy Bypass -Command
                "New-Item -ItemType Junction -Path '${GX_ASSET_DIR}' -Target '${GX_MAIN_ASSETS_PATH}' -Force" VERBATIM)
    else ()
        add_custom_command(TARGET ${GX_CURRENT_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E create_symlink ${GX_MAIN_ASSETS_PATH} ${GX_ASSET_DIR} VERBATIM)
    endif (GX_IN_WASM)

    add_dependencies(${GX_CURRENT_TARGET} GearoenixAssets)

    gx_common_target_config(${GX_CURRENT_TARGET})
endfunction(gx_prepare_target)

set(GX_ENGINE_SRC_DIR ${GX_ENGINE_SRC_DIR} PARENT_SCOPE)
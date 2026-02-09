# Gearoenix Vulkan Shader Compilation Module
# Automatically compiles GLSL shaders to SPIR-V with dependency tracking

find_program(GX_GLSLC glslc HINTS $ENV{VULKAN_SDK}/bin ${Vulkan_INCLUDE_DIR}/../bin)

if(NOT GX_GLSLC)
    message(FATAL_ERROR "glslc not found. Please install Vulkan SDK or set VULKAN_SDK environment variable.")
endif()

message(STATUS "Found glslc: ${GX_GLSLC}")

option(GX_VK_SHADER_DEBUG "Compile Vulkan shaders with debug info for NSight/RenderDoc" ON)

if(GX_VK_SHADER_DEBUG)
    set(GX_VK_SHADER_OPT_FLAGS -g -O0)
    message(STATUS "Vulkan shaders: DEBUG mode (debug info, no optimization)")
else()
    set(GX_VK_SHADER_OPT_FLAGS -O)
    message(STATUS "Vulkan shaders: RELEASE mode (maximum performance optimization)")
endif()

set(GX_VK_SHADER_GLSL_DIR "${CMAKE_CURRENT_LIST_DIR}/shader/glsl")
set(GX_VK_SHADER_OUTPUT_DIR "${GX_MAIN_ASSETS_PATH}/vulkan/shader")
set(GX_VK_SHADER_DEPS_DIR "${CMAKE_BINARY_DIR}/shader_deps")

file(MAKE_DIRECTORY ${GX_VK_SHADER_OUTPUT_DIR})
file(MAKE_DIRECTORY ${GX_VK_SHADER_DEPS_DIR})

file(GLOB GX_VK_SHADER_SOURCES CONFIGURE_DEPENDS
    "${GX_VK_SHADER_GLSL_DIR}/*.vert"
    "${GX_VK_SHADER_GLSL_DIR}/*.frag"
    "${GX_VK_SHADER_GLSL_DIR}/*.comp"
    "${GX_VK_SHADER_GLSL_DIR}/*.geom"
    "${GX_VK_SHADER_GLSL_DIR}/*.tesc"
    "${GX_VK_SHADER_GLSL_DIR}/*.tese"
    "${GX_VK_SHADER_GLSL_DIR}/*.rgen"
    "${GX_VK_SHADER_GLSL_DIR}/*.rchit"
    "${GX_VK_SHADER_GLSL_DIR}/*.rmiss"
    "${GX_VK_SHADER_GLSL_DIR}/*.rahit"
    "${GX_VK_SHADER_GLSL_DIR}/*.rint"
    "${GX_VK_SHADER_GLSL_DIR}/*.rcall"
    "${GX_VK_SHADER_GLSL_DIR}/*.mesh"
    "${GX_VK_SHADER_GLSL_DIR}/*.task"
)

set(GX_VK_COMPILED_SHADERS "")

foreach(SHADER_SOURCE ${GX_VK_SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME)

    # Strip "gx-vk-shd-" prefix from output name
    string(REGEX REPLACE "^gx-vk-shd-" "" OUTPUT_NAME ${SHADER_NAME})

    set(OUTPUT_FILE "${GX_VK_SHADER_OUTPUT_DIR}/${OUTPUT_NAME}.spv")
    set(DEP_FILE "${GX_VK_SHADER_DEPS_DIR}/${OUTPUT_NAME}.d")

    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${GX_GLSLC}
            -MD -MF ${DEP_FILE}
            -I${GX_VK_SHADER_GLSL_DIR}
            --target-env=vulkan1.4
            ${GX_VK_SHADER_OPT_FLAGS}
            -o ${OUTPUT_FILE}
            ${SHADER_SOURCE}
        DEPENDS ${SHADER_SOURCE}
        DEPFILE ${DEP_FILE}
        COMMENT "Compiling shader: ${SHADER_NAME} -> ${OUTPUT_NAME}.spv"
        VERBATIM
    )

    list(APPEND GX_VK_COMPILED_SHADERS ${OUTPUT_FILE})
endforeach()

add_custom_target(GearoenixVulkanShaders ALL DEPENDS ${GX_VK_COMPILED_SHADERS})

message(STATUS "Configured ${CMAKE_LIST_LENGTH}${GX_VK_SHADER_SOURCES} Vulkan shaders for compilation")
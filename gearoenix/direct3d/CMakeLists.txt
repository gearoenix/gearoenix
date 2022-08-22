SET(GX_ENGINE_D3D_SRC_DIR ${GX_ENGINE_SRC_DIR}/direct3d)
SET(GX_ENGINE_D3D_SRC
    ${GX_ENGINE_D3D_SRC_DIR}/shaders/gx-d3d-shd-common.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-adapter.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-adapter.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-buffer.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-buffer.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-build-configuration.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-camera.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-camera.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-check.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-command.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-command.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-descriptor.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-descriptor.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-device.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-device.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-engine.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-engine.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-loader.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-mesh.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-mesh.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-model.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-model.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-pipeline.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-pipeline.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-queue.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-queue.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-shader.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-shader.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-submission.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-submission.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-swapchain.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-swapchain.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-texture.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-texture.hpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-uploader.cpp
    ${GX_ENGINE_D3D_SRC_DIR}/gx-d3d-uploader.hpp
    PARENT_SCOPE)
SET(GX_D3D_SHADERS_DIR ${GX_ENGINE_D3D_SRC_DIR}/shaders CACHE INTERNAL "GX_D3D_SHADERS_DIR")

# To facilitate automatic shader compilation during the build, a custom-target is defined.
# This target depends on all the .hlsl files in the "shaders" folder. Furthermore, .hlsl file 
# depends on ALL the .hpp or .hlsli files which are placed in the same folder. This approach 
# has the following implications:
#  - Dependencies from .hlsl files to other files which aren't in the same folder are unaccounted 
#  for. Subsequently, changes to mentioned dependencies won't trigger a shader compilation.
#  - Changes to .hpp or .hlsli files can trigger potentially unnecessary shader compilations
#  - .hlsl to .hlsl dependencies are disregarded. As a workaround, common functionality can be placed
#  in a .hlsli file
#
# Finally, shader type (i.e. vs/hs/ds/ps/cs) is inferred from the file name. Therefore, following
# naming convention is required: .*.<shader-type>.hlsl
FUNCTION(GX_D3D_PREPARE_ASSETS)
    SET(GX_D3D_SHADERS
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-gbuffers-filler.ps.hlsl
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-gbuffers-filler.vs.hlsl
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-mipmap-generator.cs.hlsl)
    SET(GX_D3D_SHADER_INCLUDES
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-common.hpp
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-gbuffers-filler.hlsli)
    SET(GX_D3D_SHADER_LIB_DIR ${GX_MAIN_ASSETS_PATH}/d3d12-shaders)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${GX_D3D_SHADER_LIB_DIR})
    FIND_PROGRAM(DXC dxc PATHS ${GX_DXC_BINARY_DIR} REQUIRED NO_DEFAULT_PATH)
    FOREACH(GX_D3D_SHADER_FILE ${GX_D3D_SHADERS})
        GET_FILENAME_COMPONENT(GX_D3D_FILE_NAME ${GX_D3D_SHADER_FILE} NAME)
        GET_FILENAME_COMPONENT(GX_D3D_FILE_NAME_WLE ${GX_D3D_SHADER_FILE} NAME_WLE)
        STRING(REGEX REPLACE "[^.]*\\." "" GX_D3D_SHADER_TYPE ${GX_D3D_FILE_NAME_WLE})
        SET(GX_D3D_SHADER_OUT ${GX_D3D_SHADER_LIB_DIR}/${GX_D3D_FILE_NAME_WLE}.cso)
        ADD_CUSTOM_COMMAND(
            TARGET GearoenixAssets PRE_BUILD
            COMMAND ${DXC} -Qembed_debug -Qstrip_reflect -nologo -Zi -Od -T ${GX_D3D_SHADER_TYPE}_6_5 -E main -Fo ${GX_D3D_SHADER_OUT} ${GX_D3D_SHADER_FILE}
            DEPENDS ${GX_D3D_SHADER_INCLUDES} ${GX_D3D_SHADER_FILE}
            COMMENT "Compiling shader ${GX_D3D_FILE_NAME}..."
            COMMAND_EXPAND_LISTS
            VERBATIM)
        SET_SOURCE_FILES_PROPERTIES(${GX_D3D_SHADER_OUT} PROPERTIES GENERATED TRUE)
        SET(GX_D3D_SHADER_OUT_ALL ${GX_D3D_SHADER_OUT_ALL} ${GX_D3D_SHADER_OUT})
    ENDFOREACH()
    ADD_CUSTOM_TARGET(GearoenixDirectx12Shaders ALL DEPENDS ${GX_D3D_SHADER_OUT_ALL})
    ADD_DEPENDENCIES(GearoenixAssets GearoenixDirectx12Shaders)
    SET(GX_D3D_SHADER_OUT_ALL ${GX_D3D_SHADER_OUT_ALL} CACHE INTERNAL "GX_D3D_SHADER_OUT_ALL")
    SET(GX_D3D_SHADER_LIB_DIR ${GX_D3D_SHADER_LIB_DIR} CACHE INTERNAL "GX_D3D_SHADER_LIB_DIR")
ENDFUNCTION()

FUNCTION(GX_D3D_PREPARE_TARGET GX_CURRENT_TARGET)
    MESSAGE(STATUS "Preparing target ${GX_CURRENT_TARGET} in D3D render backend.")
    SET(GX_D3D_SHADER_DIR $<TARGET_FILE_DIR:${GX_CURRENT_TARGET}>/assets/d3d12-shaders)
    ADD_CUSTOM_COMMAND(
        TARGET ${GX_CURRENT_TARGET} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${GX_D3D_SHADER_DIR})
    FILE(GLOB GX_ASSET_FILES ${GX_D3D_SHADER_LIB_DIR}/*)
    FOREACH(GX_D3D_SHADER_FILE ${GX_ASSET_FILES})
        ADD_CUSTOM_COMMAND(
            TARGET ${GX_CURRENT_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GX_D3D_SHADER_FILE} ${GX_D3D_SHADER_DIR})
    ENDFOREACH()
ENDFUNCTION(GX_D3D_PREPARE_TARGET)
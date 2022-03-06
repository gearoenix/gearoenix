SET(GX_ENGINE_DXR_SRC_DIR ${GX_ENGINE_SRC_DIR}/dxr)
SET(GX_ENGINE_DXR_SRC
        ${GX_ENGINE_DXR_SRC_DIR}/shaders/gx-dxr-shd-common.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-adapter.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-adapter.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-buffer.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-buffer.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-build-configuration.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-camera.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-camera.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-check.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-command.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-command.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-descriptor.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-descriptor.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-device.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-device.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-engine.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-engine.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-loader.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-mesh.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-mesh.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-model.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-model.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-pipeline.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-pipeline.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-queue.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-queue.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-shader.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-shader.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-submission.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-submission.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-swapchain.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-swapchain.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-texture.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-texture.hpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-uploader.cpp
        ${GX_ENGINE_DXR_SRC_DIR}/gx-dxr-uploader.hpp
        PARENT_SCOPE)

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
SET(GX_DXR_SHADERS_DIR ${GX_ENGINE_DXR_SRC_DIR}/shaders)
SET(GX_DXR_SHADERS_DIR ${GX_DXR_SHADERS_DIR} CACHE INTERNAL "GX_DXR_SHADERS_DIR")

FUNCTION(GX_DXR_PREPARE_TARGET GX_CURRENT_TARGET)
    MESSAGE(STATUS "Preparing target ${GX_CURRENT_TARGET} in DXR render backend.")
    SET(GX_DXR_SHADERS
        ${GX_DXR_SHADERS_DIR}/gx-dxr-shd-gbuffers-filler.ps.hlsl
        ${GX_DXR_SHADERS_DIR}/gx-dxr-shd-gbuffers-filler.vs.hlsl
        ${GX_DXR_SHADERS_DIR}/gx-dxr-shd-mipmap-generator.cs.hlsl)
    SET(GX_DXR_SHADER_INCLUDES
        ${GX_DXR_SHADERS_DIR}/gx-dxr-shd-common.hpp
        ${GX_DXR_SHADERS_DIR}/gx-dxr-shd-gbuffers-filler.hlsli)
    SET(GX_DXR_SHADER_LIB_DIR $<TARGET_FILE_DIR:${GX_CURRENT_TARGET}>/assets/shaders)
    ADD_CUSTOM_COMMAND(
            TARGET ${GX_CURRENT_TARGET}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${GX_DXR_SHADER_LIB_DIR})
    FIND_PROGRAM(DXC dxc PATHS ${GX_DXC_BINARY_DIR} REQUIRED NO_DEFAULT_PATH)
    FOREACH(GX_DXR_SHADER_FILE ${GX_DXR_SHADERS})
        GET_FILENAME_COMPONENT(GX_DXR_FILE_NAME ${GX_DXR_SHADER_FILE} NAME)
        GET_FILENAME_COMPONENT(GX_DXR_FILE_NAME_WLE ${GX_DXR_SHADER_FILE} NAME_WLE)
        STRING(REGEX REPLACE "[^.]*\\." "" GX_DXR_SHADER_TYPE ${GX_DXR_FILE_NAME_WLE})
        SET(GX_DXR_SHADER_OUT ${GX_DXR_SHADER_LIB_DIR}/${GX_DXR_FILE_NAME_WLE}.cso)
        ADD_CUSTOM_COMMAND(
                TARGET ${GX_CURRENT_TARGET}
                COMMAND ${DXC} -Qembed_debug -Qstrip_reflect -nologo -Zi -Od -T ${GX_DXR_SHADER_TYPE}_6_5 -E main -Fo ${GX_DXR_SHADER_OUT} ${GX_DXR_SHADER_FILE}
                DEPENDS ${GX_DXR_SHADER_INCLUDES} ${GX_DXR_SHADER_FILE}
                COMMENT "Compiling shader ${GX_DXR_FILE_NAME}..."
                COMMAND_EXPAND_LISTS
                VERBATIM)
        SET_SOURCE_FILES_PROPERTIES(${GX_DXR_SHADER_OUT} PROPERTIES GENERATED TRUE)
        SET(GX_DXR_SHADER_OUT_ALL ${GX_DXR_SHADER_OUT})
    ENDFOREACH()
    SET(GX_DXR_SHADERS_TARGET "GX_DXR_SHADERS_TARGET_${GX_CURRENT_TARGET}")
    ADD_CUSTOM_TARGET(${GX_DXR_SHADERS_TARGET} ALL DEPENDS ${GX_DXR_SHADER_OUT_ALL})
    ADD_DEPENDENCIES(${GX_CURRENT_TARGET} ${GX_DXR_SHADERS_TARGET})
ENDFUNCTION(GX_DXR_PREPARE_TARGET)
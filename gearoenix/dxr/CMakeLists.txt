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
FILE(GLOB GX_DXR_SHADERS ${GX_ENGINE_DXR_SRC_DIR}/shaders/*)

FOREACH(SHADER_FILE ${GX_DXR_SHADERS})
	GET_FILENAME_COMPONENT(FILE_NAME ${SHADER_FILE} NAME)
	GET_FILENAME_COMPONENT(LAST_EXT ${FILE_NAME} EXT)

	# .hlsli or .hpp
	IF (NOT ${LAST_EXT} MATCHES  "\.(.*)\.hlsl$")
		LIST(APPEND ALL_DXR_INCLUDES ${SHADER_FILE})
	ENDIF ()
ENDFOREACH()

FOREACH(SHADER_FILE ${GX_DXR_SHADERS})
	GET_FILENAME_COMPONENT(FILE_NAME ${SHADER_FILE} NAME)
	GET_FILENAME_COMPONENT(LAST_EXT ${FILE_NAME} EXT)

	# .hlsl
	IF (${LAST_EXT} MATCHES  "\.(.*)\.hlsl$")
		LIST(APPEND ALL_DXR_INCLUDES ${SHADER_FILE})
		GX_COMPILE_SHADER(${SHADER_FILE} "${ALL_DXR_INCLUDES}" SHADER_OUT)
		LIST(POP_BACK ALL_DXR_INCLUDES)
		
		LIST(APPEND ALL_DXR_SHADERS ${SHADER_OUT})
	ENDIF ()
ENDFOREACH()

IF(MSVC)
	ADD_CUSTOM_TARGET(GX_DXR_SHADERS_TARGET ALL DEPENDS ${ALL_DXR_SHADERS})
	SET(GX_HAS_DXR_DEPENDENCY "ON" PARENT_SCOPE)
ENDIF()
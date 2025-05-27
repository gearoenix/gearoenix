set(GX_D3D_SHADERS_DIR ${GX_ENGINE_D3D_SRC_DIR}/shaders CACHE INTERNAL "GX_D3D_SHADERS_DIR")

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
function(GX_D3D_PREPARE_ASSETS)
    set(GX_D3D_SHADERS
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-gbuffers-filler.ps.hlsl
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-gbuffers-filler.vs.hlsl
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-mipmap-generator.cs.hlsl)
    set(GX_D3D_SHADER_INCLUDES
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-common.hpp
        ${GX_D3D_SHADERS_DIR}/gx-d3d-shd-gbuffers-filler.hlsli)
    set(GX_D3D_SHADER_LIB_DIR ${GX_MAIN_ASSETS_PATH}/d3d12-shaders)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${GX_D3D_SHADER_LIB_DIR})
    find_program(DXC dxc PATHS ${GX_DXC_BINARY_DIR} REQUIRED NO_DEFAULT_PATH)
    foreach(GX_D3D_SHADER_FILE ${GX_D3D_SHADERS})
        get_filename_component(GX_D3D_FILE_NAME ${GX_D3D_SHADER_FILE} NAME)
        get_filename_component(GX_D3D_FILE_NAME_WLE ${GX_D3D_SHADER_FILE} NAME_WLE)
        string(REGEX REPLACE "[^.]*\\." "" GX_D3D_SHADER_TYPE ${GX_D3D_FILE_NAME_WLE})
        set(GX_D3D_SHADER_OUT ${GX_D3D_SHADER_LIB_DIR}/${GX_D3D_FILE_NAME_WLE}.cso)
        add_custom_command(
            OUTPUT ${GX_D3D_SHADER_OUT}
            COMMAND ${DXC} -Qembed_debug -Qstrip_reflect -nologo -Zi -Od -T ${GX_D3D_SHADER_TYPE}_6_5 -E main -Fo ${GX_D3D_SHADER_OUT} ${GX_D3D_SHADER_FILE}
            DEPENDS ${GX_D3D_SHADER_INCLUDES} ${GX_D3D_SHADER_FILE}
            COMMENT "Compiling shader ${GX_D3D_FILE_NAME}..."
            COMMAND_EXPAND_LISTS
            VERBATIM)
        set_source_files_properties(${GX_D3D_SHADER_OUT} PROPERTIES GENERATED TRUE)
        set(GX_D3D_SHADER_OUT_ALL ${GX_D3D_SHADER_OUT_ALL} ${GX_D3D_SHADER_OUT})
    endforeach()
    add_custom_target(GearoenixDirectx12Shaders ALL DEPENDS ${GX_D3D_SHADER_OUT_ALL})
    add_dependencies(GearoenixAssets GearoenixDirectx12Shaders)
    set(GX_D3D_SHADER_OUT_ALL ${GX_D3D_SHADER_OUT_ALL} CACHE INTERNAL "GX_D3D_SHADER_OUT_ALL")
    set(GX_D3D_SHADER_LIB_DIR ${GX_D3D_SHADER_LIB_DIR} CACHE INTERNAL "GX_D3D_SHADER_LIB_DIR")
endfunction()

function(GX_D3D_PREPARE_TARGET GX_CURRENT_TARGET)
    message(STATUS "Preparing target ${GX_CURRENT_TARGET} in D3D render backend.")
    set(GX_D3D_SHADER_DIR $<TARGET_FILE_DIR:${GX_CURRENT_TARGET}>/assets/d3d12-shaders)
    add_custom_command(TARGET ${GX_CURRENT_TARGET} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${GX_D3D_SHADER_DIR})
    file(GLOB GX_ASSET_FILES ${GX_D3D_SHADER_LIB_DIR}/*)
    foreach(GX_D3D_SHADER_FILE ${GX_ASSET_FILES})
        add_custom_command(TARGET ${GX_CURRENT_TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GX_D3D_SHADER_FILE} ${GX_D3D_SHADER_DIR})
    endforeach()
endfunction(GX_D3D_PREPARE_TARGET)
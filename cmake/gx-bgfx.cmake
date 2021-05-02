FUNCTION(GX_PREPARE_BGFX)
    IF(GX_BGFX_ENABLED)
        SET(GX_BGFX_BASE_PATH "${GX_SDK_PATH}/bgfx")
        SET(GX_BGFX_PATH "${GX_BGFX_BASE_PATH}/bgfx")
        SET(GX_BGFX_BX_PATH "${GX_BGFX_BASE_PATH}/bx")
        SET(GX_BGFX_BIMG_PATH "${GX_BGFX_BASE_PATH}/bimg")
        GX_DOWNLOAD_FILE("https://github.com/bkaradzic/bgfx/archive/master.zip" "${GX_BGFX_PATH}")
        GX_DOWNLOAD_FILE("https://github.com/bkaradzic/bx/archive/master.zip" "${GX_BGFX_BX_PATH}")
        GX_DOWNLOAD_FILE("https://github.com/bkaradzic/bimg/archive/master.zip" "${GX_BGFX_BIMG_PATH}")
        SET(GX_BGFX_INCLUDE_DIRS ${GX_BGFX_PATH}/include)
        SET(GX_BGFX_BX_INCLUDE_DIRS ${GX_BGFX_BX_PATH}/include)
        INCLUDE_DIRECTORIES(${GX_BGFX_INCLUDE_DIRS})
        INCLUDE_DIRECTORIES(${GX_BGFX_BX_INCLUDE_DIRS})
        SET(GX_BGFX_BUILD_PATH ${GX_BGFX_PATH}/.build)
        IF(NOT EXISTS ${GX_BGFX_BUILD_PATH})
            IF(APPLE)
                SET(GX_BGFX_GENIE_EXE "${GX_BGFX_BX_PATH}/tools/bin/darwin/genie")
                SET(GX_BGFX_GENIE_ARG "xcode")
            ELSEIF(LINUX)
                SET(GX_BGFX_GENIE_EXE "${GX_BGFX_BX_PATH}/tools/bin/linux/genie")
                SET(GX_BGFX_GENIE_ARG "gmake")
            ELSEIF(MSVC)
                EXECUTE_PROCESS(
                        COMMAND ${GX_BGFX_BX_PATH}/tools/bin/windows/genie.exe "--with-tools" "vs2019"
                        WORKING_DIRECTORY ${GX_BGFX_PATH})
                SET(GX_BGFX_SLN_PATH ${GX_BGFX_BUILD_PATH}/projects/vs2019)
                FILE(GLOB GX_BGFX_PRO_FILES "${GX_BGFX_SLN_PATH}/*.vcxproj")
                FOREACH(GX_BGFX_PRO_FILE ${GX_BGFX_PRO_FILES})
                    FILE(READ ${GX_BGFX_PRO_FILE} GX_BGFX_PFC)
                    STRING(
                            REPLACE
                            "<RuntimeLibrary>MultiThreaded</RuntimeLibrary>"
                            "<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>"
                            GX_BGFX_PFC "${GX_BGFX_PFC}")
                    STRING(
                            REPLACE
                            "<RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>"
                            "<RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>"
                            GX_BGFX_PFC "${GX_BGFX_PFC}")
                    # STRING(REPLACE "_SECURE_SCL=0;" "" GX_BGFX_PFC "${GX_BGFX_PFC}")
                    # STRING(REPLACE "_SCL_SECURE=0;" "" GX_BGFX_PFC "${GX_BGFX_PFC}")
                    FILE(WRITE ${GX_BGFX_PRO_FILE} "${GX_BGFX_PFC}")
                ENDFOREACH()
                FOREACH(GX_BGFX_PRO_FILE ${GX_BGFX_PRO_FILES})
                    EXECUTE_PROCESS(
                            COMMAND ${GX_MSBUILD_EXE} ${GX_BGFX_PRO_FILE} "-t:Build" "-p:Configuration=Release"
                            "-p:Platform=x64" "-p:RuntimeLibrary=MD" WORKING_DIRECTORY ${GX_BGFX_SLN_PATH})
                    EXECUTE_PROCESS(
                            COMMAND ${GX_MSBUILD_EXE} ${GX_BGFX_PRO_FILE} "-t:Build" "-p:Configuration=Debug"
                            "-p:Platform=x64" "-p:RuntimeLibrary=MD" WORKING_DIRECTORY ${GX_BGFX_SLN_PATH})
                ENDFOREACH()
            ELSE()
                MESSAGE(FATAL_ERROR "Unsupported platform")
            ENDIF()
        ENDIF()
        IF(MSVC)
            SET(GX_BGFX_BIN_DIR ${GX_BGFX_BUILD_PATH}/win64_vs2019/bin)
            SET(GX_DEBUG_LIBS ${GX_DEBUG_LIBS}
                    ${GX_BGFX_BIN_DIR}/bgfxDebug.lib
                    # ${GX_BGFX_BIN_DIR}/bimg_decodeDebug.lib
                    ${GX_BGFX_BIN_DIR}/bimgDebug.lib
                    ${GX_BGFX_BIN_DIR}/bxDebug.lib
                    PARENT_SCOPE)
            SET(GX_RELEASE_LIBS ${GX_RELEASE_LIBS}
                    ${GX_BGFX_BIN_DIR}/bgfxRelease.lib
                    # ${GX_BGFX_BIN_DIR}/bimg_decodeRelease.lib
                    ${GX_BGFX_BIN_DIR}/bimgRelease.lib
                    ${GX_BGFX_BIN_DIR}/bxRelease.lib
                    PARENT_SCOPE)
        ELSE()
            MESSAGE(FATAL_ERROR "Unsupported platform")
        ENDIF()
    ENDIF()
ENDFUNCTION()
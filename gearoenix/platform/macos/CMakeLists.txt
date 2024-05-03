SET(GX_ENGINE_PLATFORM_MACOS_SRC_DIR ${GX_ENGINE_PLATFORM_SRC_DIR}/macos)

IF (APPLE)
    SET(GX_ENGINE_PLATFORM_MACOS_SRC
            ${GX_ENGINE_PLATFORM_MACOS_SRC_DIR}/gx-plt-mac-app-delegate.hpp
            ${GX_ENGINE_PLATFORM_MACOS_SRC_DIR}/gx-plt-mac-app-delegate.mm
            ${GX_ENGINE_PLATFORM_MACOS_SRC_DIR}/gx-plt-mac-application.hpp
            ${GX_ENGINE_PLATFORM_MACOS_SRC_DIR}/gx-plt-mac-application.mm
            ${GX_ENGINE_PLATFORM_MACOS_SRC_DIR}/gx-plt-mac-key.hpp
            ${GX_ENGINE_PLATFORM_MACOS_SRC_DIR}/gx-plt-mac-key.mm
            ${GX_ENGINE_PLATFORM_MACOS_SRC_DIR}/gx-plt-mac-view-controller.hpp
            ${GX_ENGINE_PLATFORM_MACOS_SRC_DIR}/gx-plt-mac-view-controller.mm
            PARENT_SCOPE)
ELSE ()
    SET(GX_ENGINE_PLATFORM_MACOS_SRC "" PARENT_SCOPE)
ENDIF (APPLE)

SET(GX_ENGINE_PLATFORM_MACOS_SRC_DIR ${GX_ENGINE_PLATFORM_MACOS_SRC_DIR} PARENT_SCOPE)
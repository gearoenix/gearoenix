SET(GX_ENGINE_PLATFORM_SRC_DIR ${GX_ENGINE_SRC_DIR}/platform)
SET(GX_ENGINE_PLATFORM_SRC
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/android/gx-plt-and-application.cpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/android/gx-plt-and-application.hpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/android/gx-plt-and-log.cpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/android/gx-plt-and-log.hpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/android/gx-plt-gl-context.cpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/linux/gx-plt-lnx-application.cpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/linux/gx-plt-lnx-application.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/linux/gx-plt-lnx-log.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/macro/gx-plt-mcr-disable-warnings.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/macro/gx-plt-mcr-enable-warnings.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/macro/gx-plt-mcr-lock.hpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/stream/gx-plt-stm-asset.cpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/stream/gx-plt-stm-asset.hpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/stream/gx-plt-stm-local.cpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/stream/gx-plt-stm-local.hpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/stream/gx-plt-stm-memory.cpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/stream/gx-plt-stm-memory.hpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/stream/gx-plt-stm-stream.cpp
#        ${GX_ENGINE_PLATFORM_SRC_DIR}/stream/gx-plt-stm-stream.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/windows/gx-plt-win-application.cpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/windows/gx-plt-win-application.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/windows/gx-plt-win-log.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/windows/gx-plt-win-main-entry.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-application.cpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-application.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-arguments.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-build-configuration.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-library.cpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-library.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-log.cpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-log.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-main-entry.hpp
        ${GX_ENGINE_PLATFORM_SRC_DIR}/gx-plt-runtime-configuration.hpp
        PARENT_SCOPE)
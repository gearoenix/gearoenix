SET(GX_ENGINE_AUDIO_SRC_DIR ${GX_ENGINE_SRC_DIR}/audio)
SET(GX_ENGINE_AUDIO_SRC
        ${GX_ENGINE_AUDIO_SRC_DIR}/sdl/gx-au-sdl-engine.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/sdl/gx-au-sdl-engine.hpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-audio.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-audio.hpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-engine.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-engine.hpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-manager.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-manager.hpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-player.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-player.hpp)
SET(GX_ENGINE_AUDIO_SRC ${GX_ENGINE_AUDIO_SRC} PARENT_SCOPE)
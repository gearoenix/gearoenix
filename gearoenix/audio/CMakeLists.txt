SET(GX_ENGINE_AUDIO_SRC_DIR ${GX_ENGINE_SRC_DIR}/audio)
SET(GX_ENGINE_AUDIO_SRC
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-audio.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-audio.hpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-manager.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-manager.hpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-music.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-music.hpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-speaker.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-speaker.hpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-type.cpp
        ${GX_ENGINE_AUDIO_SRC_DIR}/gx-au-type.hpp)
SET(GX_ENGINE_AUDIO_SRC ${GX_ENGINE_AUDIO_SRC} PARENT_SCOPE)
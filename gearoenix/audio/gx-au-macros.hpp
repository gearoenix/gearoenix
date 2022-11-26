#ifndef GEAROENIX_AUDIO_MACROS_HPP
#define GEAROENIX_AUDIO_MACROS_HPP
#include "../platform/gx-plt-log.hpp"

#define GX_AUDIO_FMOD_RESULT_CHECK(x)                                     \
    if (const auto _result_##__LINE__ = x; FMOD_OK != _result_##__LINE__) \
    GX_LOG_F("FMOD error: " << FMOD_ErrorString(_result_##__LINE__))

#endif
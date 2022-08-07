#ifndef GEAROENIX_AUDIO_AUDIO_HPP
#define GEAROENIX_AUDIO_AUDIO_HPP
#include <string>

namespace FMOD {
class Sound;
}

namespace gearoenix::audio {
struct Audio final {
    FMOD::Sound* const sound = nullptr;
    const std::string name;
};
}
#endif

#pragma once
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

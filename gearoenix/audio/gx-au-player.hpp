#pragma once
#include <cstdint>
#include <string>

namespace FMOD {
class Channel;
}

namespace gearoenix::audio {
struct Player final {
    const std::string name;
    const std::uint64_t audio_index;
    FMOD::Channel* const channel;
    bool is_loop = true;
};
}
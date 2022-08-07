#ifndef GEAROENIX_AUDIO_PLAYER_HPP
#define GEAROENIX_AUDIO_PLAYER_HPP
#include <string>

namespace FMOD {
class Channel;
}

namespace gearoenix::audio {
struct Player final {
    const std::string name;
    const std::size_t audio_index;
    FMOD::Channel* const channel;
    bool is_loop = true;
};
}
#endif
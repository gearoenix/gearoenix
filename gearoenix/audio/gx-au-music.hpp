#ifndef GEAROENIX_AUDIO_MUSIC_HPP
#define GEAROENIX_AUDIO_MUSIC_HPP
#include "gx-au-audio.hpp"

namespace gearoenix::audio {
struct Music : public Audio {
private:
public:
    Music(core::Id my_id, std::string name, const std::shared_ptr<platform::stream::Stream>& f);
};
}
#endif
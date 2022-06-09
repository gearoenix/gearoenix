#ifndef GEAROENIX_AUDIO_SPEAKER_HPP
#define GEAROENIX_AUDIO_SPEAKER_HPP
#include "gx-au-audio.hpp"

namespace gearoenix::audio {
struct Speaker : public Audio {
private:
public:
    Speaker(core::Id my_id, std::string name, const std::shared_ptr<platform::stream::Stream>& f);
};
}
#endif
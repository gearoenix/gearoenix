#ifndef GEAROENIX_AUDIO_SPEAKER_HPP
#define GEAROENIX_AUDIO_SPEAKER_HPP
#include "au-audio.hpp"
namespace gearoenix::audio {
class Speaker : public Audio {
private:
public:
    Speaker(core::Id my_id, std::string name, const std::shared_ptr<system::stream::Stream>& f);
};
}
#endif
#ifndef GEAROENIX_AUDIO_MUSIC_HPP
#define GEAROENIX_AUDIO_MUSIC_HPP
#include "au-audio.hpp"
namespace gearoenix::audio {
class Music : public Audio {
private:
public:
    Music(core::Id my_id, std::string name, const std::shared_ptr<system::stream::Stream>& f);
};
}
#endif
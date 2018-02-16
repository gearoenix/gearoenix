#ifndef GEAROENIX_AUDIO_MUSIC_HPP
#define GEAROENIX_AUDIO_MUSIC_HPP
#include "au-audio.hpp"
namespace gearoenix {
namespace audio {
    class Music : public Audio {
    private:
    public:
        Music(system::stream::Stream* f);
    };
} // namespace audio
} // namespace gearoenix
#endif // GEAROENIX_AUDIO_MUSIC_HPP

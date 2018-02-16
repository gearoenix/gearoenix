#ifndef GEAROENIX_AUDIO_SPEAKER_HPP
#define GEAROENIX_AUDIO_SPEAKER_HPP
#include "au-audio.hpp"
namespace gearoenix {
namespace audio {
    class Speaker : public Audio {
    private:
    public:
        Speaker(system::stream::Stream* f);
    };
} // namespace audio
} // namespace gearoenix
#endif // GEAROENIX_AUDIO_SPEAKER_HPP

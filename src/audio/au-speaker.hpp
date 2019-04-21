#ifndef GEAROENIX_AUDIO_SPEAKER_HPP
#define GEAROENIX_AUDIO_SPEAKER_HPP
#include "au-audio.hpp"
namespace gearoenix {
namespace audio {
    class Speaker : public Audio {
    private:
    public:
        Speaker(core::Id my_id, const std::shared_ptr<system::stream::Stream>& f);
    };
} // namespace audio
} // namespace gearoenix
#endif // GEAROENIX_AUDIO_SPEAKER_HPP

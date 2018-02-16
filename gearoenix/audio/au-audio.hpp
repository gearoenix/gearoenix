#ifndef GEAROENIX_AUDIO_AUDIO_HPP
#define GEAROENIX_AUDIO_AUDIO_HPP
#include "../core/asset/cr-asset.hpp"
namespace gearoenix {
namespace system {
namespace file {
    class File;
}
}
namespace audio {
    class Audio : public core::asset::Asset {
    private:
    protected:
        Audio(system::file::File* f);

    public:
        virtual ~Audio();
        static Audio* read(system::file::File* f);
    };
} // namespace audio
} // namespace gearoenix
#endif // GEAROENIX_AUDIO_AUDIO_HPP

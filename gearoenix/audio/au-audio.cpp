#include "au-audio.hpp"
#include "../system/sys-file.hpp"
#include "au-music.hpp"
#include "au-speaker.hpp"

gearoenix::audio::Audio::Audio(system::File* f)
{
    std::vector<uint8_t> bytes;
    f->read(bytes);
    LOGE("TODO");
}

gearoenix::audio::Audio::~Audio() {}

gearoenix::audio::Audio* gearoenix::audio::Audio::read(system::File* f)
{
    switch (f->read<core::Id>()) {
    case 10:
        return new Music(f);
    case 20:
        return new Speaker(f);
    }
    LOGF("Unexpected");
}

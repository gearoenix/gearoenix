#include "au-audio.hpp"
#include "../system/stream/sys-stm-stream.hpp"
#include "../system/sys-log.hpp"
#include "au-music.hpp"
#include "au-speaker.hpp"

gearoenix::audio::Audio::Audio(system::stream::Stream* f)
{
    std::vector<uint8_t> bytes;
    f->read(bytes);
    TODO;
}

gearoenix::audio::Audio::~Audio() {}

gearoenix::audio::Audio* gearoenix::audio::Audio::read(system::stream::Stream* f)
{
    core::Id audio_type;
    f->read(audio_type);
    switch (audio_type) {
    case 10:
        return new Music(f);
    case 20:
        return new Speaker(f);
    }
    UNEXPECTED;
}

#include "au-audio.hpp"
#include "../system/stream/sys-stm-stream.hpp"
#include "../system/sys-log.hpp"
#include "au-music.hpp"
#include "au-speaker.hpp"

gearoenix::audio::Audio::Audio(core::Id my_id, system::stream::Stream* f)
    : core::asset::Asset(my_id, core::asset::Asset::AssetType::AUDIO)
{
    std::vector<uint8_t> bytes;
    f->read(bytes);
    GXTODO;
}

gearoenix::audio::Audio::~Audio() {}

gearoenix::audio::Audio* gearoenix::audio::Audio::read(core::Id my_id, system::stream::Stream* f)
{
    core::Id audio_type;
    f->read(audio_type);
    switch (audio_type) {
    case 10:
        return new Music(my_id, f);
    case 20:
        return new Speaker(my_id, f);
    }
    GXUNEXPECTED;
}

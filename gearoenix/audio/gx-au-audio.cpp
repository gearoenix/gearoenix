#include "gx-au-audio.hpp"
#include "../system/gx-sys-log.hpp"
#include "../system/stream/gx-sys-stm-stream.hpp"
#include "gx-au-music.hpp"
#include "gx-au-speaker.hpp"

gearoenix::audio::Audio::Audio(const core::Id my_id, std::string n, const std::shared_ptr<system::stream::Stream>& f) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Audio, std::move(n))
{
    std::vector<uint8_t> bytes;
    f->read(bytes);
    GXTODO
}

gearoenix::audio::Audio::~Audio() noexcept = default;

gearoenix::audio::Audio* gearoenix::audio::Audio::read(const core::Id my_id, std::string name, const std::shared_ptr<system::stream::Stream>& f) noexcept
{
    core::Id audio_type;
    f->read(audio_type);
    switch (audio_type) {
    case 10:
        return new Music(my_id, std::move(name), f);
    case 20:
        return new Speaker(my_id, std::move(name), f);
    default:
        GX_UNEXPECTED
    }
}

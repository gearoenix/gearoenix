#include "gx-au-audio.hpp"
#include "../platform/gx-plt-log.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "gx-au-music.hpp"
#include "gx-au-speaker.hpp"

gearoenix::audio::Audio::Audio(const core::Id my_id, std::string n, const std::shared_ptr<platform::stream::Stream>& f) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Audio, std::move(n))
{
    std::vector<uint8_t> bytes;
    f->read(bytes);
    GXTODO
}

gearoenix::audio::Audio::~Audio() noexcept = default;

gearoenix::audio::Audio* gearoenix::audio::Audio::read(const core::Id my_id, std::string name, const std::shared_ptr<platform::stream::Stream>& f) noexcept
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

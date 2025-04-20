#include "gx-au-manager.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "gx-au-audio.hpp"
#include "gx-au-engine.hpp"
#include "gx-au-macros.hpp"

#if !GX_PLATFORM_WEBASSEMBLY
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#endif

gearoenix::audio::Manager::Manager(Engine& engine)
    : engine(engine)
{
}

std::uint64_t gearoenix::audio::Manager::create_audio(const std::string& asset_path, const std::string& name)
{
    return create_audio(platform::stream::Path::create_asset(asset_path), name);
}

std::uint64_t gearoenix::audio::Manager::create_audio(const platform::stream::Path& asset_path, const std::string& name)
{
    const auto stream = platform::stream::Stream::open(asset_path);
    return create_audio(*stream, name);
}

std::uint64_t gearoenix::audio::Manager::create_audio(platform::stream::Stream& asset_stream, const std::string& name)
{
    const auto data = asset_stream.get_file_content();
    return create_audio_ogg(data, name);
}

std::uint64_t gearoenix::audio::Manager::create_audio_ogg(const std::vector<std::uint8_t>& ogg_data, const std::string& name)
{

#if !GX_PLATFORM_WEBASSEMBLY
    FMOD_CREATESOUNDEXINFO info;
    GX_SET_ZERO(info);
    info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    info.length = static_cast<decltype(info.length)>(ogg_data.size());
    FMOD::Sound* sound = nullptr;
    engine.get_system()->createSound(
        reinterpret_cast<const char*>(ogg_data.data()),
        FMOD_CREATESAMPLE | FMOD_OPENMEMORY | FMOD_LOOP_NORMAL,
        &info,
        &sound);
    std::lock_guard _ignore_lg(this_lock);
    const auto audio_index = audios.size();
    audios.push_back(Audio { sound, name });
    audio_name_map.emplace(name, audio_index);
    return static_cast<std::uint64_t>(audio_index);
#else
    return 0;
#endif
}

std::uint64_t gearoenix::audio::Manager::create_player(const std::string& name, const std::uint64_t audio_index, const bool is_loop)
{
#if !GX_PLATFORM_WEBASSEMBLY
    FMOD::Channel* channel = nullptr;
    GX_AUDIO_FMOD_RESULT_CHECK(engine.get_system()->playSound(audios[audio_index].sound, nullptr, true, &channel));
    if (is_loop) {
        GX_AUDIO_FMOD_RESULT_CHECK(channel->setLoopCount(-1));
    }
    std::lock_guard _ignore_lg(this_lock);
    const auto player_index = players.size();
    players.push_back(Player { name, audio_index, channel, is_loop });
    return player_index;
#else
    return 0;
#endif
}

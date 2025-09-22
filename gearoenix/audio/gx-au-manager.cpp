#include "gx-au-manager.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "gx-au-audio.hpp"
#include "gx-au-engine.hpp"
#include "gx-au-macros.hpp"

gearoenix::audio::Manager::Manager()
    : Singleton(this)
{
}

gearoenix::audio::Manager::~Manager() = default;

std::uint64_t gearoenix::audio::Manager::create_audio(std::string&& asset_path, const std::string& name)
{
    return create_audio(platform::stream::Path::create_asset(std::move(asset_path)), name);
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

std::uint64_t gearoenix::audio::Manager::create_audio_ogg(const std::vector<std::uint8_t>&, const std::string&)
{
    GX_UNIMPLEMENTED;
}

std::uint64_t gearoenix::audio::Manager::create_player(const std::string&, const std::uint64_t, const bool)
{
    GX_UNIMPLEMENTED;
}

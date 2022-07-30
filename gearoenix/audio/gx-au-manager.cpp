#include "gx-au-manager.hpp"
#include "gx-au-audio.hpp"

#include "../platform/macro/gx-plt-mcr-disable-warnings.hpp"
#include "../platform/macro/gx-plt-mcr-enable-warnings.hpp"
#include <stb_vorbis.c>

gearoenix::audio::Manager::Manager(platform::Application& platform_application) noexcept
    : platform_application(platform_application)
{
}

void gearoenix::audio::Manager::fill_samples(std::size_t player_index, std::vector<std::uint16_t>& samples) noexcept
{
    fill_samples(players[player_index], samples);
}

void gearoenix::audio::Manager::fill_samples(Player& player, std::vector<std::uint16_t>& samples) noexcept
{
    if (!player.enabled)
        return;
    auto& aud_samples = audios[player.audio_index].samples;
    const double scale = player.channel_scale;
    constexpr std::uint32_t dom = 10000;
    const auto nom = static_cast<std::uint32_t>(scale * static_cast<double>(dom));
    for (std::size_t sample_index = 0; sample_index < samples.size(); ++sample_index, ++player.sample_index) {
        // Ugly but for now
        if (aud_samples.size() <= player.sample_index) {
            if (player.wrap == Wrap::Once)
                return;
            player.sample_index = 0;
        }
        samples[sample_index] += static_cast<std::uint16_t>((static_cast<std::uint32_t>(aud_samples[player.sample_index]) * nom) / dom);
        samples[sample_index] = aud_samples[player.sample_index];
    }
}

std::size_t gearoenix::audio::Manager::create_audio(const std::string& asset_path, const std::string& name) noexcept
{
    return create_audio(platform::stream::Path::create_asset(asset_path), name);
}

std::size_t gearoenix::audio::Manager::create_audio(const platform::stream::Path& asset_path, const std::string& name) noexcept
{
    auto stream = platform::stream::Stream::open(asset_path, platform_application);
    return create_audio(*stream, name);
}

std::size_t gearoenix::audio::Manager::create_audio(platform::stream::Stream& asset_stream, const std::string& name) noexcept
{
    const auto data = asset_stream.get_file_content();
    return create_audio_ogg(data, name);
}

std::size_t gearoenix::audio::Manager::create_audio_ogg(const std::vector<std::uint8_t>& ogg_data, const std::string& name) noexcept
{
    int channels_count = 0, sample_rate = 0;
    short* output = nullptr;
    const auto result = stb_vorbis_decode_memory(
        ogg_data.data(),
        static_cast<int>(ogg_data.size()),
        &channels_count,
        &sample_rate,
        &output);
    GX_ASSERT(result > 0);
    GX_ASSERT(output != nullptr);
    std::vector<std::uint16_t> samples(result);
    for (int sample_index = 0; sample_index < result; ++sample_index)
        samples[sample_index] = static_cast<std::uint16_t>(static_cast<int>(output[sample_index]) + (1 << 15));
    std::free(output);
    std::lock_guard<std::mutex> _ignore_lg(this_lock);
    const auto audio_index = audios.size();
    audios.emplace_back(samples, name);
    audio_name_map.emplace(name, audio_index);
    return audio_index;
}

std::size_t gearoenix::audio::Manager::create_player(
    const std::string& name,
    const std::size_t audio_index,
    const float channel_scale,
    const Wrap wrap) noexcept
{
    std::lock_guard<std::mutex> _ignore_lg(this_lock);
    const auto player_index = players.size();
    players.push_back(Player {
        .name = name,
        .audio_index = audio_index,
        .channel_scale = channel_scale,
        .wrap = wrap,
    });
    return player_index;
}

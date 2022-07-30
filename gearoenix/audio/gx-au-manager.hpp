#ifndef GEAROENIX_AUDIO_MANAGER_HPP
#define GEAROENIX_AUDIO_MANAGER_HPP
#include "../core/gx-cr-types.hpp"
#include "../core/sync/gx-cr-sync-end-caller.hpp"
#include "../platform/stream/gx-plt-stm-path.hpp"
#include "gx-au-player.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/static_vector.hpp>
#include <string>
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::audio {
struct Audio;
struct Manager {
    platform::Application& platform_application;
    std::mutex this_lock;
    std::vector<Audio> audios;
    boost::container::flat_map<std::string, std::size_t> audio_name_map;
    std::vector<Player> players;
    boost::container::flat_map<std::string, std::size_t> player_name_map;
    boost::container::static_vector<math::Vec3<float>, 16> listners; // For future not used right now

    Manager(platform::Application& platform_application) noexcept;
    void fill_samples(std::size_t player_index, std::vector<std::uint16_t>& samples) noexcept;
    void fill_samples(Player& player, std::vector<std::uint16_t>& samples) noexcept;
    [[nodiscard]] std::size_t create_audio(const std::string& asset_path, const std::string& name) noexcept;
    [[nodiscard]] std::size_t create_audio(const platform::stream::Path& asset_path, const std::string& name) noexcept;
    [[nodiscard]] std::size_t create_audio(platform::stream::Stream& asset_stream, const std::string& name) noexcept;
    [[nodiscard]] std::size_t create_audio_ogg(const std::vector<std::uint8_t>& ogg_data, const std::string& name) noexcept;
    [[nodiscard]] std::size_t create_player(
        const std::string& name,
        std::size_t audio_index,
        float channel_scale = 1.0f,
        Wrap wrap = Wrap::Loop) noexcept;
};
}
#endif

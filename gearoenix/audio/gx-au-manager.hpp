#pragma once
#include "../core/job/gx-cr-job-end-caller.hpp"
#include "../platform/stream/gx-plt-stm-path.hpp"
#include "gx-au-audio.hpp"
#include "gx-au-player.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/static_vector.hpp>
#include <mutex>
#include <string>
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::audio {
struct Engine;
struct Manager {
    Engine& engine;
    std::mutex this_lock;
    std::vector<Audio> audios;
    boost::container::flat_map<std::string, std::uint64_t> audio_name_map;
    std::vector<Player> players;
    boost::container::flat_map<std::string, std::uint64_t> player_name_map;

    explicit Manager(Engine& engine);
    [[nodiscard]] std::uint64_t create_audio(const std::string& asset_path, const std::string& name);
    [[nodiscard]] std::uint64_t create_audio(const platform::stream::Path& asset_path, const std::string& name);
    [[nodiscard]] std::uint64_t create_audio(platform::stream::Stream& asset_stream, const std::string& name);
    [[nodiscard]] std::uint64_t create_audio_ogg(const std::vector<std::uint8_t>& ogg_data, const std::string& name);
    [[nodiscard]] std::uint64_t create_player(const std::string& name, std::uint64_t audio_index, bool is_loop = true);
};
}
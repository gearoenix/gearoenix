#ifndef GEAROENIX_AUDIO_PLAYER_HPP
#define GEAROENIX_AUDIO_PLAYER_HPP
#include "../math/gx-math-vector-3d.hpp"
#include "gx-au-audio.hpp"
#include "gx-au-wrap.hpp"
#include <optional>
#include <string>

namespace gearoenix::audio {
struct Player final {
    const std::string name;
    const std::size_t audio_index;
    std::optional<math::Vec3<float>> location = std::nullopt;
    std::size_t sample_index = 0;
    float channel_scale = 1.0f;
    Wrap wrap = Wrap::Loop;
    bool enabled = true;
};
}
#endif
#ifndef GEAROENIX_AUDIO_AUDIO_HPP
#define GEAROENIX_AUDIO_AUDIO_HPP
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::audio {
struct Audio final {
    const std::vector<std::uint16_t> samples;
    const std::string name;
};
}
#endif

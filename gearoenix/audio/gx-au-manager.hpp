#ifndef GEAROENIX_AUDIO_MANAGER_HPP
#define GEAROENIX_AUDIO_MANAGER_HPP
#include "../core/gx-cr-types.hpp"
#include "../core/sync/gx-cr-sync-end-caller.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::audio {
struct Audio;
struct Manager {

public:
    Manager() noexcept;
    ~Manager() noexcept = default;
};
}
#endif

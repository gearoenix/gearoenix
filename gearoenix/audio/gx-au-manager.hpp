#ifndef GEAROENIX_AUDIO_MANAGER_HPP
#define GEAROENIX_AUDIO_MANAGER_HPP
#include "../core/cache/gx-cr-cache-file.hpp"
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
protected:
    render::engine::Engine* const e;
    core::cache::File<Audio> cache;

public:
    Manager(std::unique_ptr<platform::stream::Stream> s, render::engine::Engine* e) noexcept;
    ~Manager() noexcept = default;
    std::shared_ptr<Audio> get_gx3d(core::Id mid, core::sync::EndCaller<Audio>& c) noexcept;
};
}
#endif

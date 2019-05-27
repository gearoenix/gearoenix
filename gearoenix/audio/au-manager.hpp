#ifndef GEAROENIX_AUDIO_MANAGER_HPP
#define GEAROENIX_AUDIO_MANAGER_HPP
#include "../core/cache/cr-cache-file.hpp"
#include "../core/cr-types.hpp"
#include "../core/sync/cr-sync-end-caller.hpp"
namespace gearoenix {
namespace render::engine {
        class Engine;
    }
namespace system::stream {
        class Stream;
    }
namespace audio {
    class Audio;
    class Manager {
    protected:
        render::engine::Engine*const e;
        core::cache::File<Audio> cache;

    public:
        Manager(system::stream::Stream* s, render::engine::Engine* e) noexcept;
        ~Manager() noexcept = default;
        std::shared_ptr<Audio> get_gx3d(core::Id mid, core::sync::EndCaller<Audio>& c) noexcept;
    };
} // namespace audio
} // namespace gearoenix
#endif

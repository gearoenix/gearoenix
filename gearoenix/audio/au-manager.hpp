#ifndef GEAROENIX_AUDIO_MANAGER_HPP
#define GEAROENIX_AUDIO_MANAGER_HPP
#include "../core/cr-types.hpp"
#include "../core/sync/cr-sync-end-caller.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace engine {
        class Engine;
    }
}
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace audio {
    class Audio;
    class Manager {
    protected:
        const std::shared_ptr<render::engine::Engine> e;
        const std::shared_ptr<system::stream::Stream> s;

    public:
        Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<render::engine::Engine>& e);
        ~Manager();
        std::shared_ptr<Audio> get(core::Id mid, core::sync::EndCaller<Audio> c);
    };
} // namespace audio
} // namespace gearoenix
#endif

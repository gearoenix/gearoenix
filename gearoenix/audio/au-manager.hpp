#ifndef GEAROENIX_AUDIO_MANAGER_HPP
#define GEAROENIX_AUDIO_MANAGER_HPP
#include <memory>
#include "../core/cr-types.hpp"
#include "../core/sync/cr-sync-end-caller.hpp"

namespace gearoenix {
namespace render {
class Engine;
}
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace audio {
class Audio;
class Manager
{
protected:
    render::Engine *e;
    const std::shared_ptr<system::stream::Stream> s;
public:
    Manager(const std::shared_ptr<system::stream::Stream>& s, render::Engine* e);
    ~Manager();
    std::shared_ptr<Audio> get(core::Id mid, core::sync::EndCaller<Audio> c);

};
} // namespace audio
} // namespace gearoenix
#endif

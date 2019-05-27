#ifndef GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#define GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix {
namespace system::stream {
        class Stream;
    }
namespace render {
    namespace engine {
        class Engine;
    }
    namespace skybox {
        class Skybox;
        class Manager {
        protected:
            engine::Engine*const e;
            core::cache::File<Skybox> cache;

        public:
            Manager(system::stream::Stream* s, engine::Engine* e) noexcept ;
            ~Manager() noexcept = default;
            std::shared_ptr<Skybox> get(core::Id mid, const core::sync::EndCaller<Skybox> c) noexcept ;
        };
    }
}
}
#endif

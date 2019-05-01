#ifndef GEAROEMIX_RENDER_SKYBOX_MANAGER_HPP
#define GEAROEMIX_RENDER_SKYBOX_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace skybox {
        class Skybox;
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
            core::cache::File<Skybox> cache;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();
            std::shared_ptr<Skybox> get(const core::Id mid, const core::sync::EndCaller<Skybox> c);
        };
    }
}
}
#endif

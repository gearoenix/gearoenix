#ifndef GEAROEMIX_RENDER_CAMERA_MANAGER_HPP
#define GEAROEMIX_RENDER_CAMERA_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"

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
    namespace camera {
        class Camera;
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
            core::cache::File<Camera> cache;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();
            std::shared_ptr<Camera> get_gx3d(const core::Id cid, core::sync::EndCaller<Camera>& call);
        };
    }
}
}
#endif

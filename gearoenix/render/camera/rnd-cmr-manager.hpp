#ifndef GEAROEMIX_RENDER_CAMERA_MANAGER_HPP
#define GEAROEMIX_RENDER_CAMERA_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"

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
            core::cache::File<Camera> cached;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();
        };
    }
}
}
#endif

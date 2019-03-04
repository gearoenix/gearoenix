#ifndef GEAROEMIX_RENDER_CAMERA_MANAGER_HPP
#define GEAROEMIX_RENDER_CAMERA_MANAGER_HPP
#include <memory>

namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
class Engine;
    namespace camera {
        class Manager {
        protected:
            Engine *e;
            const std::shared_ptr<system::stream::Stream> s;
        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, Engine* e);
            ~Manager();
        };
    }
}
}
#endif

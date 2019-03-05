#ifndef GEAROEMIX_RENDER_SKYBOX_MANAGER_HPP
#define GEAROEMIX_RENDER_SKYBOX_MANAGER_HPP
#include <memory>

namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    class Engine;
    namespace skybox {
        class Manager {
        private:
            Engine* e;
            const std::shared_ptr<system::stream::Stream> s;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, Engine* e);
            ~Manager();
        };
    }
}
}
#endif

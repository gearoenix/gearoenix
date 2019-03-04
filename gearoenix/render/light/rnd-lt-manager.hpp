#ifndef GEAROEMIX_RENDER_LIGHT_MANAGER_HPP
#define GEAROEMIX_RENDER_LIGHT_MANAGER_HPP
#include <memory>

namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    class Engine;
    namespace light {
        class Manager {
        private:
            const std::shared_ptr<system::stream::Stream> s;
        public:
        Manager(const std::shared_ptr<system::stream::Stream>& s);
            ~Manager();
        };
    }
}
}
#endif

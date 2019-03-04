#ifndef GEAROEMIX_RENDER_LIGHT_MANAGER_HPP
#define GEAROEMIX_RENDER_LIGHT_MANAGER_HPP
#include <memory>
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"

namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    class Engine;
    namespace light {
    class Light;
        class Manager {
        protected:
            Engine *e = nullptr;
            const std::shared_ptr<system::stream::Stream> s;
        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, Engine* e);
            ~Manager();
            std::shared_ptr<Light> get(core::Id mid, core::sync::EndCaller<Light> c);
        };
    }
}
}
#endif

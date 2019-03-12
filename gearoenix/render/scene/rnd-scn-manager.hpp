#ifndef GEAROEMIX_RENDER_SCENE_MANAGER_HPP
#define GEAROEMIX_RENDER_SCENE_MANAGER_HPP
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../core/cache/cr-cache-file.hpp"
#include <memory>

namespace gearoenix {
	namespace core {
		namespace sync {
			class WorkWaiter;
		}
	}
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace scene {
        class Scene;
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
			core::cache::File<Scene> cache;
			const std::shared_ptr<core::sync::WorkWaiter> io_worker;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();
			/// It is gonna load the scene (if exists) in another thread.
            void get_gx3d(const core::Id mid, core::sync::EndCaller<Scene> c);
        };
    }
}
}
#endif

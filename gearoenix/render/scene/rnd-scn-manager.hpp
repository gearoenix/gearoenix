#ifndef GEAROENIX_RENDER_SCENE_MANAGER_HPP
#define GEAROENIX_RENDER_SCENE_MANAGER_HPP
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-scn-scene.hpp"
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
            /// T must be derived from Scene and have the same constructor that Scene has.
            template <typename T>
            typename std::enable_if<std::is_base_of<Scene, T>::value, std::shared_ptr<T>>::type
            create(core::sync::EndCaller<T>& c);
            const std::map<core::Id, std::weak_ptr<scene::Scene>>& get_scenes() const;
        };
    }
}
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::scene::Scene, T>::value, std::shared_ptr<T>>::type
gearoenix::render::scene::Manager::create(core::sync::EndCaller<T>& c)
{
    const core::Id id = core::asset::Manager::create_id();
    const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
    const std::shared_ptr<T> result(new T(id, e, call));
    c.set_data(result);
    const std::weak_ptr<Scene> w = result;
    cache.get_cacher().get_cacheds()[id] = w;
    return result;
}

#endif

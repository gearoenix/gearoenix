#ifndef GEAROEMIX_RENDER_CAMERA_MANAGER_HPP
#define GEAROEMIX_RENDER_CAMERA_MANAGER_HPP
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-cmr-camera.hpp"

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
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
			core::cache::File<Camera> cache;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();
            std::shared_ptr<Camera> get_gx3d(const core::Id cid, core::sync::EndCaller<Camera>& call);
			template <typename T>
			typename std::enable_if<std::is_base_of<Camera, T>::value, std::shared_ptr<T>>::type create();
        };
    }
}
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::camera::Camera, T>::value, std::shared_ptr<T>>::type
gearoenix::render::camera::Manager::create()
{
	const core::Id id = core::asset::Manager::create_id();
	const std::shared_ptr<T> result(new T(id, e));
	const std::weak_ptr<Camera> w = result;
	cache.get_cacher().get_cacheds()[id] = w;
	return result;
}
#endif

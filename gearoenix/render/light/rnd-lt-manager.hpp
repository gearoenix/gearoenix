#ifndef GEAROEMIX_RENDER_LIGHT_MANAGER_HPP
#define GEAROEMIX_RENDER_LIGHT_MANAGER_HPP
#include "../../core/asset/cr-asset-manager.hpp"
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
    namespace light {
        class Light;
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
			core::cache::File<Light> cache;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();
            std::shared_ptr<Light> get_gx3d(const core::Id mid, core::sync::EndCaller<Light>& c);
			template <typename T>
			typename std::enable_if<std::is_base_of<gearoenix::render::light::Light, T>::value, std::shared_ptr<T>>::type
				create() noexcept;
        };
    }
}
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::light::Light, T>::value, std::shared_ptr<T>>::type
gearoenix::render::light::Manager::create() noexcept
{
	const core::Id id = core::asset::Manager::create_id();
	const std::shared_ptr<T> result(new T(id, e));
	const std::weak_ptr<Light> w = result;
	cache.get_cacher().get_cacheds()[id] = w;
	return result;
}
#endif

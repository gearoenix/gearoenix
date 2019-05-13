#ifndef GEAROEMIX_RENDER_MODEL_MANAGER_HPP
#define GEAROEMIX_RENDER_MODEL_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "rnd-mdl-model.hpp"
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
    namespace model {
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
            core::cache::File<Model> cache;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();
            std::shared_ptr<Model> get_gx3d(const core::Id mid, core::sync::EndCaller<Model>& c);
			/// T must be derived from Model and have the same constructor that Model has.
			template <typename T>
			typename std::enable_if<std::is_base_of<Model, T>::value, std::shared_ptr<T>>::type
			create(core::sync::EndCaller<T>& c);
        };
    }
}
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::model::Model, T>::value, std::shared_ptr<T>>::type
gearoenix::render::model::Manager::create(core::sync::EndCaller<T>& c) {
	const core::Id id = core::asset::Manager::create_id();
	const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
	const std::shared_ptr<T> result(new T(id, e, call));
	c.set_data(result);
	cache.get_cacher().get_cacheds()[id] = result;
	return result;
}

#endif
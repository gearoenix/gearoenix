#ifndef GEAROENIX_RENDER_LIGHT_MANAGER_HPP
#define GEAROENIX_RENDER_LIGHT_MANAGER_HPP
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix {
namespace system::stream {
    class Stream;
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace light {
        class Light;
        class Manager {
        protected:
            engine::Engine* const e;
            core::cache::File<Light> cache;

        public:
            Manager(system::stream::Stream* s, engine::Engine* e) noexcept;
            ~Manager() noexcept = default;
            std::shared_ptr<Light> get_gx3d(core::Id mid, core::sync::EndCaller<Light>& c) noexcept;
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

#ifndef GEAROENIX_RENDER_LIGHT_MANAGER_HPP
#define GEAROENIX_RENDER_LIGHT_MANAGER_HPP
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/cache/gx-cr-cache-file.hpp"
#include "../../core/gx-cr-types.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <memory>

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::light {
class Light;
class Manager {
protected:
    engine::Engine* const e;
    core::cache::File<Light> cache;

public:
    Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* e) noexcept;
    ~Manager() noexcept = default;
    std::shared_ptr<Light> get_gx3d(core::Id mid, core::sync::EndCaller<Light>& c) noexcept;
    template <typename T>
    typename std::enable_if<std::is_base_of<gearoenix::render::light::Light, T>::value, std::shared_ptr<T>>::type
    create(std::string name) noexcept;
};
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::light::Light, T>::value, std::shared_ptr<T>>::type
gearoenix::render::light::Manager::create(std::string name) noexcept
{
    const core::Id id = core::asset::Manager::create_id();
    const std::shared_ptr<T> result(new T(id, std::move(name), e));
    const std::weak_ptr<Light> w = result;
    cache.get_cacher().get_cacheds()[id] = w;
    return result;
}
#endif

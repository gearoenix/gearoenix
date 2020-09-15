#ifndef GEAROENIX_RENDER_MANAGER_MANAGER_HPP
#define GEAROENIX_RENDER_MANAGER_MANAGER_HPP
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/cache/gx-cr-cache-file.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "gx-rnd-rfl-reflection.hpp"

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::reflection {
class Manager {
protected:
    engine::Engine* const e;
    core::cache::File<Reflection> cache;

public:
    Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* e) noexcept;
    ~Manager() noexcept;
    std::shared_ptr<Reflection> get_gx3d(core::Id id, core::sync::EndCaller<Reflection>& call) noexcept;
    template <typename T>
    typename std::enable_if<std::is_base_of<Reflection, T>::value, std::shared_ptr<T>>::type
    create(std::string name, core::sync::EndCaller<T>& c) noexcept;
};
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::reflection::Reflection, T>::value, std::shared_ptr<T>>::type
gearoenix::render::reflection::Manager::create(std::string name, core::sync::EndCaller<T>& c) noexcept
{
    const core::Id id = core::asset::Manager::create_id();
    const std::shared_ptr<T> result(new T(id, std::move(name), e, core::sync::EndCaller<core::sync::EndCallerIgnore>([c] {})));
    const std::weak_ptr<Reflection> w = result;
    cache.get_cacher().get_cacheds()[id] = w;
    c.set_data(result);
    return result;
}

#endif

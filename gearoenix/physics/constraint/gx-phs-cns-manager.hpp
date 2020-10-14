#ifndef GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_MANAGER_HPP
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/cache/gx-cr-cache-file.hpp"
#include "../../core/gx-cr-static.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::physics::constraint {
class Constraint;
class Manager {
    GX_GET_CPTR_PRV(render::engine::Engine, e)
    GX_GET_CREF_PRV(core::cache::File<Constraint>, cache)

public:
    Manager(std::unique_ptr<system::stream::Stream> s, render::engine::Engine* e) noexcept;
    ~Manager() noexcept;
    std::shared_ptr<Constraint> get_gx3d(core::Id mid, core::sync::EndCaller<Constraint>& c) noexcept;
    template <typename T>
    [[nodiscard]] typename std::enable_if<std::is_base_of<Constraint, T>::value, std::shared_ptr<T>>::type
    create(std::string name) noexcept;
};
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::physics::constraint::Constraint, T>::value, std::shared_ptr<T>>::type
gearoenix::physics::constraint::Manager::create(std::string name) noexcept
{
    const core::Id id = core::asset::Manager::create_id();
    const std::shared_ptr<T> result(new T(id, std::move(name), e));
    cache.get_cacher().get_cacheds().emplace(id, result);
    return result;
}
#endif

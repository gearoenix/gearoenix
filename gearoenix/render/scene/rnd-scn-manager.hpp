#ifndef GEAROENIX_RENDER_SCENE_MANAGER_HPP
#define GEAROENIX_RENDER_SCENE_MANAGER_HPP
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-scn-scene.hpp"
#include <memory>

namespace gearoenix::core::sync {
class WorkWaiter;
}

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::scene {
class Manager {
    GX_GET_PTRC_PRV(engine::Engine, e)
    GX_GET_CREF_PRV(core::cache::File<Scene>, cache)
    GX_GET_UCPTR_PRV(core::sync::WorkWaiter, io_worker)

public:
    Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* e) noexcept;
    ~Manager() noexcept;
    /// It is gonna load the scene (if exists) in another thread.
    void get_gx3d(core::Id mid, core::sync::EndCaller<Scene> c) noexcept;
    /// It is gonna load the scene (if exists) in another thread.
    void get_gx3d(const std::string& name, const core::sync::EndCaller<Scene>& c) noexcept;
    /// T must be derived from Scene and have the same constructor that Scene has.
    template <typename T>
    typename std::enable_if<std::is_base_of<Scene, T>::value, std::shared_ptr<T>>::type
    create(core::sync::EndCaller<T>& c) noexcept;
    [[nodiscard]] const std::map<core::Id, std::weak_ptr<scene::Scene>>& get_scenes() const noexcept;
};
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::scene::Scene, T>::value, std::shared_ptr<T>>::type
gearoenix::render::scene::Manager::create(core::sync::EndCaller<T>& c) noexcept
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

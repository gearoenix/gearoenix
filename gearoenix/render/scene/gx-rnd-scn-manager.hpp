#ifndef GEAROENIX_RENDER_SCENE_MANAGER_HPP
#define GEAROENIX_RENDER_SCENE_MANAGER_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include <map>
#include <memory>
#include <string>

namespace gearoenix::core::sync {
struct WorkWaiter;
}

namespace gearoenix::render::gltf {
struct Loader;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::scene {
struct Manager final {
    typedef std::map<std::string, core::ecs::Entity::id_t> NameToIdMap;
    typedef std::map<core::ecs::Entity::id_t, std::string> IdToNameMap;

    GX_GET_PTRC_PRV(engine::Engine, e)
    GX_GET_UCPTR_PRV(core::sync::WorkWaiter, io_worker)
    GX_GET_UPTR_PRV(gltf::Loader, loader)
    GX_GET_VAL_PRV(core::ecs::Entity::id_t, default_scene, 0)
    GX_GET_CREF_PRV(NameToIdMap, name_to_id)
    GX_GET_CREF_PRV(IdToNameMap, id_to_name)

    void load_gltf_worker(platform::stream::Path, const core::sync::EndCallerIgnored& c) noexcept;

public:
    explicit Manager(engine::Engine* e) noexcept;
    ~Manager() noexcept;
    void load_gltf(platform::stream::Path, core::sync::EndCallerIgnored c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
};
}
#endif

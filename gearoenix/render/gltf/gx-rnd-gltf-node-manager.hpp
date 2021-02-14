#ifndef GEAROENIX_RENDER_GLTF_NODE_MANAGER_HPP
#define GEAROENIX_RENDER_GLTF_NODE_MANAGER_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <map>

namespace gearoenix::render::gltf {
struct Loader;
struct NodeManager final {
private:
    Loader& loader;
    std::map<std::size_t, core::ecs::Entity::id_t> loaded_nodes;
    std::map<std::string, core::ecs::Entity::id_t> loaded_nodes_names;

public:
    explicit NodeManager(Loader& loader) noexcept;

    [[nodiscard]] core::ecs::Entity::id_t get(std::size_t, const core::sync::EndCallerIgnored& c) noexcept;
};
}

#endif
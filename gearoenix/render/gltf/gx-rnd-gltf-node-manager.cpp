#include "gx-rnd-gltf-node-manager.hpp"
#include "gx-rnd-gltf-loader.hpp"
#include <tiny_gltf.h>

gearoenix::render::gltf::NodeManager::NodeManager(Loader& loader) noexcept
    : loader(loader)
{
}

gearoenix::core::ecs::Entity::id_t gearoenix::render::gltf::NodeManager::get(
    const std::size_t index, const core::sync::EndCallerIgnored& c) noexcept
{

    auto index_search = loaded_nodes.find(index);
    if (loaded_nodes.end() != index_search)
        return index_search->second;

    const auto& data = *loader.get_data();
    const auto& nodes = data.nodes;
    const auto& node = nodes[index];

    GX_UNIMPLEMENTED

    return 0;
}

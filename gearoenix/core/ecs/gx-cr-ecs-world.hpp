#ifndef GEAROENIX_CORE_ECS_WORLD_HPP
#define GEAROENIX_CORE_ECS_WORLD_HPP

#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "gx-cr-ecs-types.hpp"
#include <array>
#include <typeindex>
#include <vector>

namespace gearoenix::core::ecs {
struct World {
private:
    typedef std::uint32_t component_index_t;
    struct EntityInfo {
        entity_id_t id = 0;
        std::vector<component_index_t> components_indices;
    };
    GX_CREATE_GUARD(this)
    entity_id_t latest_entity_id = 0;
    std::map<std::type_index, std::vector<std::uint8_t>> components_map;
    std::map<std::vector<std::type_index>, std::vector<EntityInfo>> archetypes;
    std::map<entity_id_t, std::map<std::type_index, component_index_t>> entities_to_archetypes;

public:
    template <typename... ComponentsTypes>
    [[nodiscard]] entity_id_t create_entity(ComponentsTypes&&... components) noexcept;
    template <typename ComponentType>
    [[nodiscard]] ComponentType& get_component(entity_id_t entity_id) noexcept;
};
}

template <typename... ComponentsTypes>
gearoenix::core::ecs::entity_id_t gearoenix::core::ecs::World::create_entity(ComponentsTypes&&... components) noexcept
{
    constexpr auto cms_sz = sizeof...(components);
    std::map<std::type_index, component_index_t> archetype_indices;
    GX_GUARD_LOCK(this)
    (([&]<typename T>(T&& component) {
        const auto type_index = std::type_index(typeid(component));
        auto& vec = components_map[type_index];
        const auto index = vec.size();
        archetype_indices[type_index] = index;
        vec.resize(index + sizeof(T));
        for (auto i = index; i < vec.size(); ++i)
            vec[i] = 0;
        auto com = reinterpret_cast<T*>(&vec[index]);
        *com = std::forward<T>(component);
    }(std::move(components))),
        ...);
    ++latest_entity_id;
    std::vector<std::type_index> archetypes_index;
    archetypes_index.reserve(cms_sz);
    std::vector<component_index_t> indices;
    indices.reserve(cms_sz);
    for (const auto& ti : archetype_indices) {
        archetypes_index.push_back(ti.first);
        indices.push_back(ti.second);
    }
    archetypes[std::move(archetypes_index)].push_back(EntityInfo {
        .id = latest_entity_id,
        .components_indices = std::move(indices),
    });
    entities_to_archetypes[latest_entity_id] = std::move(archetype_indices);
    return latest_entity_id;
}

template <typename ComponentType>
ComponentType& gearoenix::core::ecs::World::get_component(entity_id_t entity_id) noexcept
{
    const auto ti = std::type_index(typeid(ComponentType));
    const auto ci = entities_to_archetypes[entity_id][ti];
    return *reinterpret_cast<ComponentType*>(components_map[ti].data() + ci);
}

#endif
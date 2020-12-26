#ifndef GEAROENIX_CORE_ECS_WORLD_HPP
#define GEAROENIX_CORE_ECS_WORLD_HPP

#include "gx-cr-ecs-types.hpp"
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include <typeindex>
#include <array>

namespace gearoenix::core::ecs {
    struct World {
    private:
        typedef component_index_t std::uint32_t;
        struct EntityInfo {
            entity_id_t id = 0;
            std::vector<component_index_t> components_indices;
        };
        GX_CREATE_GUARD(this)
        entity_id_t latest_entity_id = 0;
        std::map<std::type_index, std::vector<std::uint8_t>> components_map;
        std::map<std::vector<std::type_index>, std::vector<EntityInfo>> archetypes;
        std::map<entity_id_t, std::vector<std::type_index>> entities_to_archetypes;
    public:
        template<typename... ComponentsTypes>
        [[nodiscard]] entity_id_t create_entity(ComponentsTypes&&... components) noexcept;
    };
}

template<typename... ComponentsTypes>
gearoenix::core::ecs::entity_id_t gearoenix::core::ecs::World::create_entity(ComponentsTypes&&.. components) noexcept {
    constexpr auto cms_sz = sizeof...(components);
    std::vector<std::type_index> archetypes_index;
    archetypes_index.reserve(cms_sz);
    std::vector<component_index_t> indices;
    indices.reserve(cms_sz);
    GX_GUARD_LOCK(this)
    ((
            auto& vec = components_map[std::type_index(typeid(components))];
            archetypes_index.push_back(std::type_index(typeid(components)));
            const auto index = vec.size();
            indices.push_back(index);
            vec.resize(index + sizeof(components));
            for(auto i = index; i < vec.size(); ++i) vec[i] = 0;
            auto com = reinterpret_cast<decltype(components)*>(&vec[index]);
            *com = std::move(components);
            ), ...);
    ++latest_entity_id;
    archetypes[archetypes_index].push_back(EntityInfo { .id = latest_entity_id, .components_indices = indices });
    entities_to_archetypes[latest_entity_id] = archetypes_index;
    return latest_entity_id;
}

#endif
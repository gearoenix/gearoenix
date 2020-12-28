#ifndef GEAROENIX_CORE_ECS_WORLD_HPP
#define GEAROENIX_CORE_ECS_WORLD_HPP

#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "gx-cr-ecs-types.hpp"
#include <array>
#include <functional>
#include <map>
#include <string>
#include <typeindex>
#include <vector>

namespace gearoenix::core::ecs {
/// The World of ECS
///
/// Main focus of this struct is performance of systems and memory usage
struct World {
private:
    typedef std::uint32_t component_index_t;
    GX_CREATE_GUARD(this)
    entity_id_t latest_entity_id = 0;
    // TODO: benchmark unordered map in future
    std::map<std::type_index, std::vector<std::uint8_t>> components_map;
    std::map<std::vector<std::type_index>, std::vector<std::uint8_t>> archetypes;
    std::map<entity_id_t, std::map<std::type_index, component_index_t>> entities_to_archetypes;

public:
    template <typename... ComponentsTypes>
    [[nodiscard]] entity_id_t create_entity(ComponentsTypes&&... components) noexcept;
    template <typename ComponentType>
    [[nodiscard]] ComponentType& get_component(entity_id_t entity_id) noexcept;
    template <typename... ComponentsTypes>
    void parallel_system(const std::function<void(entity_id_t entity, ComponentsTypes&... components)>& fun) noexcept;
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
    auto& archetype = archetypes[std::move(archetypes_index)];
    const auto archetype_index = archetype.size();
    archetype.resize(archetype_index + sizeof(entity_id_t) + cms_sz * sizeof(component_index_t));
    auto* const ent_ptr = archetype.data() + archetype_index;
    *reinterpret_cast<entity_id_t*>(ent_ptr) = latest_entity_id;
    auto* cms_ptr = ent_ptr + sizeof(entity_id_t);
    for (const component_index_t index : indices) {
        *reinterpret_cast<component_index_t*>(cms_ptr) = index;
        cms_ptr += sizeof(component_index_t);
    }
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

template <typename... ComponentsTypes>
void gearoenix::core::ecs::World::parallel_system(
    const std::function<void(entity_id_t, ComponentsTypes&...)>& fun) noexcept
{
    constexpr auto cms_sz = sizeof...(ComponentsTypes);
    constexpr auto not_sz = CountNot<ComponentsTypes...>::value;
    constexpr auto pos_sz = cms_sz - not_sz;
    std::type_index pos_type_indices[pos_sz];
    std::type_index not_type_indices[not_sz];
    std::size_t not_index = 0;
    std::size_t pos_index = 0;
    (([&]<typename T>(std::type_index&& ti) {
        if (IsNot<T>::value) {
            not_type_indices[not_index] = ti;
            ++not_index;
        } else {
            pos_type_indices[pos_index] = ti;
            ++pos_index;
        }
    }(std::type_index(typeid(ComponentsTypes)))),
        ...);
    for (const auto& archetype : archetypes) {
        bool pos_compatible[pos_sz] = { false };
        const auto& type_indices = archetype.first;
        for (const auto& type_index : type_indices) {
            for (const auto& not_ti : not_type_indices)
                if (not_ti == type_index)
                    goto continue_archetypes;
            for (std::size_t i = 0; i < pos_sz; ++i) {
                if (pos_type_indices[i] == type_index) {
                    pos_compatible[i] = true;
                }
            }
        }
        for (bool b : pos_compatible) {
            if (!b) {
                goto continue_archetypes;
            }
        }
        auto& entities = archetype.second;
        for (std::size_t archetype_index = 0; archetype_index < entities.size();) {
        }
    continue_archetypes:
        (void)0;
    }
}

#endif
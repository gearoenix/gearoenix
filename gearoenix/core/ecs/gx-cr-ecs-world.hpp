#ifndef GEAROENIX_CORE_ECS_WORLD_HPP
#define GEAROENIX_CORE_ECS_WORLD_HPP

#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "../gx-cr-range.hpp"
#include "gx-cr-ecs-types.hpp"
#include <algorithm>
#include <array>
#include <execution>
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
    struct SystemQueryInfo {
        const bool is_not;
        const std::type_index type_index;
        std::uint8_t* const starting_pointer;
        bool satisfied_by_archetype = false;
        std::size_t index_in_archetype = 0;
    };
    typedef std::uint32_t component_index_t;
    GX_CREATE_GUARD(this)
    entity_id_t latest_entity_id = 0;
    // TODO: benchmark unordered map in future
    std::map<std::type_index, std::vector<std::uint8_t>> components_storage;
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
        auto& vec = components_storage[type_index];
        const auto index = vec.size();
        archetype_indices[type_index] = static_cast<component_index_t>(index);
        vec.resize(index + sizeof(T));
        for (auto i = index; i < vec.size(); ++i)
            vec[i] = 0;
        auto* const com = reinterpret_cast<T*>(&vec[index]);
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
    return *reinterpret_cast<ComponentType*>(components_storage[ti].data() + ci);
}

template <typename... ComponentsTypes>
void gearoenix::core::ecs::World::parallel_system(
    const std::function<void(entity_id_t, ComponentsTypes&...)>& fun) noexcept
{
    constexpr auto all_components_count = sizeof...(ComponentsTypes);
    static_assert(all_components_count != 0, "Number of components can not be zero!");
    SystemQueryInfo query_info[all_components_count] = {
        SystemQueryInfo {
            .is_not = IsNot<ComponentsTypes>::value,
            .type_index = std::type_index(IsNot<ComponentsTypes>::value ? typeid(IsNot<ComponentsTypes>::type) : typeid(ComponentsTypes)),
            .starting_pointer = IsNot<ComponentsTypes>::value ? nullptr : components_storage[std::type_index(typeid(ComponentsTypes))].data(),
        }...,
    };
    for (const auto& archetype : archetypes) {
        {
            for (auto& info : query_info)
                info.satisfied_by_archetype = info.is_not;
            const auto& archetype_type_indices = archetype.first;
            for (std::size_t archetype_type_indices_index = 0; archetype_type_indices_index < archetype_type_indices.size(); ++archetype_type_indices_index) {
                const auto& archetype_type_index = archetype_type_indices[archetype_type_indices_index];
                for (auto& info : query_info) {
                    if (archetype_type_index == info.type_index) {
                        if (info.is_not)
                            goto continue_archetypes;
                        info.satisfied_by_archetype = true;
                        info.index_in_archetype = archetype_type_indices_index;
                    }
                }
            }
            for (auto& info : query_info) {
                if (!info.satisfied_by_archetype) {
                    goto continue_archetypes;
                }
            }
            auto& entities = archetype.second;
            const auto entity_size = sizeof(entity_id_t) + sizeof(component_index_t) * archetype_type_indices.size();
            auto range = PtrRange(entities.data(), entities.size(), entity_size);
            std::for_each(std::execution::par_unseq, range.begin(), range.end(), [&](const std::uint8_t* ptr) {
                const auto entity_id = *reinterpret_cast<const entity_id_t*>(ptr);
                ptr += sizeof(entity_id_t);
                const auto* const component_indices = reinterpret_cast<const component_index_t*>(ptr);
                std::size_t index = 0;
                fun(entity_id, [&]<typename T>(T* t) -> T& {
                    const auto& info = query_info[index];
                    ++index;
                    if (IsNot<T>::value)
                        return *t;
                    return *reinterpret_cast<T*>(&info.starting_pointer[component_indices[info.index_in_archetype]]);
                }(reinterpret_cast<ComponentsTypes*>(0))...);
            });
        }
    continue_archetypes:
        continue;
    }
}

#endif
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
    static_assert(cms_sz != 0, "Number of components can not be zero!");
    constexpr auto not_sz = CountNot<ComponentsTypes...>::value;
    constexpr auto pos_sz = cms_sz - not_sz;
    std::size_t component_index_in_archetype[cms_sz] = {
        (IsNot<ComponentsTypes>::value ? static_cast<std::size_t>(-1) : 0)...,
    };
    std::uint8_t* component_ptr_in_archetype[cms_sz] = { nullptr };
    std::uint8_t fake_pos_type_indices[pos_sz * sizeof(std::type_index)] = {};
    std::uint8_t fake_not_type_indices[not_sz * sizeof(std::type_index)] = {};
    auto* const pos_type_indices = reinterpret_cast<std::type_index*>(fake_pos_type_indices);
    auto* const not_type_indices = reinterpret_cast<std::type_index*>(fake_not_type_indices);
    std::size_t not_index = 0;
    std::size_t pos_index = 0;
    std::size_t index = 0;
    std::size_t pos_indices[pos_sz];
    (([&]<typename T>(T*) {
        const auto ti = std::type_index(typeid(T));
        if (IsNot<T>::value) {
            not_type_indices[not_index] = ti;
            ++not_index;
        } else {
            pos_type_indices[pos_index] = ti;
            component_ptr_in_archetype[index] = components_map[ti].data();
            pos_indices[pos_index] = index;
            ++pos_index;
        }
        ++index;
    }(reinterpret_cast<ComponentsTypes*>(0))),
        ...);
    for (const auto& archetype : archetypes) {
        {
            bool pos_compatible[pos_sz];
            for (std::size_t i = 0; i < pos_sz; ++i)
                pos_compatible[i] = false;
            const auto& type_indices = archetype.first;
            for (index = 0; index < type_indices.size(); ++index) {
                const auto& type_index = type_indices[index];
                for (std::size_t not_ti_i = 0; not_ti_i < not_sz; ++not_ti_i) {
                    if (not_type_indices[not_ti_i] == type_index) {
                        goto continue_archetypes;
                    }
                }
                for (std::size_t i = 0; i < pos_sz; ++i) {
                    if (pos_type_indices[i] == type_index) {
                        pos_compatible[i] = true;
                        component_index_in_archetype[pos_indices[i]] = index;
                    }
                }
            }
            for (bool b : pos_compatible) {
                if (!b) {
                    goto continue_archetypes;
                }
            }
            auto& entities = archetype.second;
            const auto entity_size = sizeof(entity_id_t) + sizeof(component_index_t) * type_indices.size();
            auto range = PtrRange(entities.data(), entities.size(), entity_size);
            std::for_each(std::execution::par_unseq, range.begin(), range.end(), [&](const std::uint8_t* ptr) {
                const auto entity_id = *reinterpret_cast<const entity_id_t*>(ptr);
                ptr += sizeof(entity_id_t);
                const auto* const com_indices = reinterpret_cast<const component_index_t*>(ptr);
                index = 0;
                void* com_ptr[cms_sz] = { nullptr };
                (([&]<typename T>(T*) {
                    const auto idx = component_index_in_archetype[index];
                    if (idx != -1) {
                        com_ptr[index] = component_ptr_in_archetype[com_indices[idx]];
                    }
                    ++index;
                }(reinterpret_cast<ComponentsTypes*>(0))),
                    ...);
                index = 0;
                fun(entity_id, (*reinterpret_cast<ComponentsTypes*>(com_ptr[index++]))...);
            });
        }
    continue_archetypes:
        continue;
    }
}

#endif
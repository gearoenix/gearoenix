#ifndef GEAROENIX_CORE_ECS_WORLD_HPP
#define GEAROENIX_CORE_ECS_WORLD_HPP
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "gx-cr-ecs-archetype.hpp"
#include "gx-cr-ecs-entity.hpp"

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
    GX_CREATE_GUARD(this)
    std::map<Archetype::id_t, Archetype> archetypes;
    // id -> (archetype, index)
    std::map<entity_id_t, std::pair<Archetype::id_t, std::size_t>> entities;

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
    ((Component::type_check<ComponentsTypes>()), ...);
    auto archetype_id = Archetype::create_id<ComponentsTypes...>();
    GX_GUARD_LOCK(this)
    ++Entity::last_id;
    auto search = archetypes.find(archetype_id);
    if (archetypes.end() == search) {
        search = archetypes.emplace(archetype_id, Archetype(components...)).first;
    }
    auto& archetype = search->second;
    const auto index = archetype.allocate(Entity::last_id, components...);
    entities.emplace(Entity::last_id, std::make_pair(archetype_id, index));
    return Entity::last_id;
}

template <typename ComponentType>
ComponentType& gearoenix::core::ecs::World::get_component(const entity_id_t id) noexcept
{
    const auto& ti = entities[id];
    return archetypes.find(ti.first)->second.get_component<ComponentType>(ti.second);
}

template <typename... ComponentsTypes>
void gearoenix::core::ecs::World::parallel_system(
    const std::function<void(entity_id_t, ComponentsTypes&...)>& fun) noexcept
{
    constexpr auto all_components_count = sizeof...(ComponentsTypes);
    static_assert(all_components_count != 0, "Number of queried components can not be zero!");
    for (auto& [a_id, archetype] : archetypes) {
        if (!Archetype::satisfy<ComponentsTypes...>(a_id))
            continue;
        archetype.parallel_system(fun);
    }
    //    SystemQueryInfo query_info[all_components_count] = {
    //        SystemQueryInfo {
    //            .is_not = IsNot<ComponentsTypes>::value,
    //            .type_index = std::type_index(IsNot<ComponentsTypes>::value ? typeid(typename IsNot<ComponentsTypes>::type) : typeid(ComponentsTypes)),
    //            .starting_pointer = IsNot<ComponentsTypes>::value ? nullptr : components_storage[std::type_index(typeid(ComponentsTypes))].data(),
    //        }...,
    //    };
    //    for (const auto& archetype : archetypes) {
    //        {
    //            for (auto& info : query_info)
    //                info.satisfied_by_archetype = info.is_not;
    //            const auto& archetype_type_indices = archetype.first;
    //            for (std::size_t archetype_type_indices_index = 0; archetype_type_indices_index < archetype_type_indices.size(); ++archetype_type_indices_index) {
    //                const auto& archetype_type_index = archetype_type_indices[archetype_type_indices_index];
    //                for (auto& info : query_info) {
    //                    if (archetype_type_index == info.type_index) {
    //                        if (info.is_not)
    //                            goto continue_archetypes;
    //                        info.satisfied_by_archetype = true;
    //                        info.index_in_archetype = archetype_type_indices_index;
    //                    }
    //                }
    //            }
    //            for (auto& info : query_info) {
    //                if (!info.satisfied_by_archetype) {
    //                    goto continue_archetypes;
    //                }
    //            }
    //            auto& entities = archetype.second;
    //            const auto entity_size = sizeof(entity_id_t) + sizeof(component_index_t) * archetype_type_indices.size();
    //            auto range = PtrRange(entities.data(), entities.size(), entity_size);
    //            std::for_each(std::execution::par_unseq, range.begin(), range.end(), [&](const std::uint8_t* ptr) {
    //                const auto entity_id = *reinterpret_cast<const entity_id_t*>(ptr);
    //                ptr += sizeof(entity_id_t);
    //                const auto* const component_indices = reinterpret_cast<const component_index_t*>(ptr);
    //                std::size_t index = 0;
    //                const auto component_maker = [&]<typename T>(T* t) -> T& {
    //                    const auto& info = query_info[index];
    //                    ++index;
    //                    if (IsNot<T>::value)
    //                        return *t;
    //                    return *reinterpret_cast<T*>(&info.starting_pointer[component_indices[info.index_in_archetype]]);
    //                };
    //                fun(entity_id, component_maker(reinterpret_cast<ComponentsTypes*>(0))...);
    //            });
    //        }
    //    continue_archetypes:
    //        continue;
    //    }
}

#endif
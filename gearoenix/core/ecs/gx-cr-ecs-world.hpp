#ifndef GEAROENIX_CORE_ECS_WORLD_HPP
#define GEAROENIX_CORE_ECS_WORLD_HPP
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "gx-cr-ecs-archetype.hpp"
#include "gx-cr-ecs-entity.hpp"
#include <variant>

namespace gearoenix::core::ecs {
/// The World of ECS
///
/// Main focus of this struct is performance of systems and memory usage
struct World {
private:
    GX_CREATE_GUARD(this)
    std::map<Archetype::id_t, Archetype> archetypes;
    // id -> (archetype, index)
    std::map<Entity::id_t, Entity> entities;

    GX_CREATE_GUARD(delayed_actions)
    // 0 - Entity addition
    // 1 - Entity deletion
    // 2 - Component(s) addition
    // 3 - Component(s) deletion
    std::vector<std::variant<
        Entity::Builder,
        Entity::id_t,
        std::pair<Entity::id_t, std::map<std::type_index, std::vector<std::uint8_t>>>,
        std::pair<Entity::id_t, std::vector<std::type_index>>>>
        delayed_actions;

public:
    //--------------------------------------Entity creation----------------------------------------------
    /// You must know your context (state of world lock), unless you want to end up being deadlocked
    [[nodiscard]] Entity::id_t create_entity_with_builder(Entity::Builder&&) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] Entity::id_t create_entity(ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        auto archetype_id = Archetype::create_id<ComponentsTypes...>();
        GX_GUARD_LOCK(this)
        const auto id = ++Entity::last_id;
        auto search = archetypes.find(archetype_id);
        if (archetypes.end() == search) {
            search = archetypes.emplace(archetype_id, Archetype::create<ComponentsTypes...>()).first;
        }
        auto& archetype = search->second;
        const auto index = archetype.allocate(id, std::forward<ComponentsTypes>(components)...);
        entities.emplace(id, Entity(std::move(archetype_id), index));
        return id;
    }

    /// Recommended way to add an entity, in case you do not know the context you're in.
    [[nodiscard]] Entity::id_t delayed_create_entity_with_builder(Entity::Builder&&) noexcept;

    template <typename... ComponentsTypes>
    [[nodiscard]] Entity::id_t delayed_create_entity(ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        Entity::Builder b;
        b.add_components(components...);
        return delayed_create_entity(std::move(b));
    }
    //--------------------------------------Entity deletion----------------------------------------------
    void delete_entity(Entity::id_t) noexcept;

    void delayed_delete_entity(Entity::id_t) noexcept;
    //--------------------------------------Component addition-------------------------------------------
    void add_components_map(Entity::id_t, const std::map<std::type_index, std::vector<std::uint8_t>>&) noexcept;

    template <typename... ComponentsTypes>
    void add_components(const Entity::id_t ei, ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        Entity::Builder b(ei);
        b.add_components(components...);
        add_components_map(ei, b.components);
    }

    void delayed_add_components_map(Entity::id_t, std::map<std::type_index, std::vector<std::uint8_t>>) noexcept;

    template <typename... ComponentsTypes>
    void delayed_add_components(const Entity::id_t ei, ComponentsTypes&&... components) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        Entity::Builder b(ei);
        b.add_components(components...);
        delayed_add_components_map(ei, std::move(b.components));
    }
    //--------------------------------------Component deletion-------------------------------------------
    template <typename... ComponentsTypes>
    void remove_components(const Entity::id_t ei) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        const std::type_index ts[] = {
            std::type_index(typeid(ComponentsTypes))...,
        };
        remove_components(ei, ts, sizeof...(ComponentsTypes));
    }

    void remove_components_list(Entity::id_t, const std::type_index*, std::size_t) noexcept;

    template <typename... ComponentsTypes>
    void delayed_remove_components(const Entity::id_t ei) noexcept
    {
        Component::types_check<ComponentsTypes...>();
        std::vector<std::type_index> ts = {
            std::type_index(typeid(ComponentsTypes))...,
        };
        delayed_remove_components_list(ei, std::move(ts));
    }

    void delayed_remove_components_list(Entity::id_t, std::vector<std::type_index>) noexcept;

    /// Does not provide a good performance, use it wisely
    template <typename ComponentType>
    [[nodiscard]] ComponentType& get_component(const Entity::id_t id) noexcept
    {
        Component::type_check<ComponentType>();
        auto entity_search = entities.find(id);
#ifdef GX_DEBUG_MODE
        if (entities.end() == entity_search)
            GX_LOG_F("Entity '" << id << "' not found.")
#endif
        const auto& e = entity_search->second;
        auto archetype_search = archetypes.find(e.archetype);
#ifdef GX_DEBUG_MODE
        if (archetypes.end() == archetype_search)
            GX_LOG_F("Archetype for entity '" << id << "' not found.")
#endif
        return archetype_search->second.get_component<ComponentType>(e.index_in_archetype);
    }

    /// Highly optimized way of doing things
    template <typename... ComponentsTypes>
    void parallel_system(const std::function<void(Entity::id_t, ComponentsTypes&...)>& fun) noexcept
    {
        Component::query_types_check<ComponentsTypes...>();
        GX_GUARD_LOCK(this)
        for (auto& [a_id, archetype] : archetypes) {
            if (!Archetype::satisfy<ComponentsTypes...>(a_id))
                continue;
            archetype.parallel_system(fun);
        }
    }

    /// It will do all the delayed actions
    void update() noexcept;

    /// It's really a time consuming operation use it rarely, but it can be really helpful for performance
    void defragment() noexcept;
};
}

#endif
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
    template <ComponentConcept... ComponentsTypes>
    [[nodiscard]] Entity::id_t create_entity(ComponentsTypes&&... components) noexcept;
    [[nodiscard]] Entity::id_t create_entity(Entity::Builder&&) noexcept;
    /// Recommended way to add an entity, in case you do not know the context you're in.
    template <ComponentConcept... ComponentsTypes>
    [[nodiscard]] Entity::id_t delayed_create_entity(ComponentsTypes&&... components) noexcept;
    [[nodiscard]] Entity::id_t delayed_create_entity(Entity::Builder&&) noexcept;
    //--------------------------------------Entity deletion----------------------------------------------
    void delete_entity(Entity::id_t) noexcept;
    void delayed_delete_entity(Entity::id_t) noexcept;
    //--------------------------------------Component addition-------------------------------------------
    template <ComponentConcept... ComponentsTypes>
    void add_components(Entity::id_t, ComponentsTypes&&... components) noexcept;
    void add_components(Entity::id_t, const std::map<std::type_index, std::vector<std::uint8_t>>&) noexcept;
    template <ComponentConcept... ComponentsTypes>
    void delayed_add_components(Entity::id_t, ComponentsTypes&&... components) noexcept;
    void delayed_add_components(Entity::id_t, std::map<std::type_index, std::vector<std::uint8_t>>) noexcept;
    //--------------------------------------Component deletion-------------------------------------------
    template <ComponentConcept... ComponentsTypes>
    void remove_components(Entity::id_t) noexcept;
    void remove_components(Entity::id_t, const std::type_index*, std::size_t) noexcept;
    template <ComponentConcept... ComponentsTypes>
    void delayed_remove_components(Entity::id_t) noexcept;
    void delayed_remove_components(Entity::id_t, std::vector<std::type_index>) noexcept;

    /// Does not provide a good performance, use it wisely
    template <ComponentConcept ComponentType>
    [[nodiscard]] ComponentType& get_component(Entity::id_t entity_id) noexcept;

    /// Highly optimized way of doing things
    template <ComponentQueryConcept... ComponentsTypes>
    void parallel_system(const std::function<void(Entity::id_t entity, ComponentsTypes&... components)>& fun) noexcept;

    /// It will do all the delayed actions
    void update() noexcept;

    /// It's really a time consuming operation use it rarely, but it can be really helpful for performance
    void defragment() noexcept;
};
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
gearoenix::core::ecs::Entity::id_t gearoenix::core::ecs::World::create_entity(ComponentsTypes&&... components) noexcept
{
    auto archetype_id = Archetype::create_id<ComponentsTypes...>();
    GX_GUARD_LOCK(this)
    const auto id = ++Entity::last_id;
    auto search = archetypes.find(archetype_id);
    if (archetypes.end() == search) {
        search = archetypes.emplace(archetype_id, Archetype(components...)).first;
    }
    auto& archetype = search->second;
    const auto index = archetype.allocate(id, components...);
    entities.emplace(id, std::make_pair(archetype_id, index));
    return id;
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
gearoenix::core::ecs::Entity::id_t gearoenix::core::ecs::World::delayed_create_entity(ComponentsTypes&&... components) noexcept
{
    Entity::Builder b;
    b.add_components(components...);
    return delayed_create_entity(std::move(b));
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
void gearoenix::core::ecs::World::add_components(const Entity::id_t ei, ComponentsTypes&&... components) noexcept
{
    Entity::Builder b(ei);
    b.add_components(components...);
    add_components(ei, b.components);
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
void gearoenix::core::ecs::World::delayed_add_components(const Entity::id_t ei, ComponentsTypes&&... components) noexcept
{
    Entity::Builder b(ei);
    b.add_components(components...);
    delayed_add_components(ei, std::move(b.components));
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
void gearoenix::core::ecs::World::remove_components(const Entity::id_t ei) noexcept
{
    const std::type_index ts[] = {
        std::type_index(typeid(ComponentsTypes))...,
    };
    remove_components(ei, ts, sizeof...(ComponentsTypes));
}

template <gearoenix::core::ecs::ComponentConcept... ComponentsTypes>
void gearoenix::core::ecs::World::delayed_remove_components(const Entity::id_t ei) noexcept
{
    std::vector<std::type_index> ts = {
        std::type_index(typeid(ComponentsTypes))...,
    };
    delayed_remove_components(ei, std::move(ts));
}

template <gearoenix::core::ecs::ComponentConcept ComponentType>
ComponentType& gearoenix::core::ecs::World::get_component(const Entity::id_t id) noexcept
{
    const auto& e = entities[id];
    return archetypes.find(e.archetype)->second.get_component<ComponentType>(e.index_in_archetype);
}

template <gearoenix::core::ecs::ComponentQueryConcept... ComponentsTypes>
void gearoenix::core::ecs::World::parallel_system(
    const std::function<void(Entity::id_t, ComponentsTypes&...)>& fun) noexcept
{
    constexpr auto all_components_count = sizeof...(ComponentsTypes);
    static_assert(all_components_count != 0, "Number of queried components can not be zero!");
    GX_GUARD_LOCK(this)
    for (auto& [a_id, archetype] : archetypes) {
        if (!Archetype::satisfy<ComponentsTypes...>(a_id))
            continue;
        archetype.parallel_system(fun);
    }
}

#endif
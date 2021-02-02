#include "gx-cr-ecs-world.hpp"

gearoenix::core::ecs::Entity::id_t gearoenix::core::ecs::World::create_entity_with_builder(Entity::Builder&& b) noexcept
{
    Archetype::id_t archetype_id;
    archetype_id.reserve(b.components.size());
    for (const auto& c : b.components) {
        archetype_id.emplace_back(c.first);
    }
    GX_GUARD_LOCK(this)
    auto search = archetypes.find(archetype_id);
    if (archetypes.end() == search) {
        search = archetypes.emplace(archetype_id, Archetype(b.components)).first;
    }
    auto& archetype = search->second;
    const auto index = archetype.allocate_entity(b.id, b.components);
    entities.emplace(b.id, Entity(std::move(archetype_id), index));
    return b.id;
}

gearoenix::core::ecs::Entity::id_t gearoenix::core::ecs::World::delayed_create_entity_with_builder(Entity::Builder&& b) noexcept
{
    const auto id = b.id;
    GX_GUARD_LOCK(delayed_actions)
    delayed_actions.emplace_back(std::move(b));
    return id;
}

void gearoenix::core::ecs::World::delete_entity(const Entity::id_t id) noexcept
{
    GX_GUARD_LOCK(this)
    const auto search = entities.find(id);
    if (entities.end() == search) {
        GX_LOG_E("Deleting an already deleted entity: " << id)
        return;
    }
    auto& e = search->second;
    archetypes.find(e.archetype)->second.delete_entity(e.index_in_archetype);
    entities.erase(search);
}

void gearoenix::core::ecs::World::delayed_delete_entity(const Entity::id_t id) noexcept
{
    GX_GUARD_LOCK(delayed_actions)
    delayed_actions.emplace_back(id);
}

void gearoenix::core::ecs::World::add_components_map(const Entity::id_t, Entity::Builder::components_t&&) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::core::ecs::World::delayed_add_components_map(const Entity::id_t ei, Entity::Builder::components_t&& cs) noexcept
{
    GX_GUARD_LOCK(delayed_actions)
    delayed_actions.emplace_back(std::make_pair(ei, std::move(cs)));
}

void gearoenix::core::ecs::World::remove_components_list(
    const Entity::id_t, const std::type_index*, std::size_t) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::core::ecs::World::delayed_remove_components_list(
    const Entity::id_t ei,
    std::vector<std::type_index>&& cs) noexcept
{
    GX_GUARD_LOCK(delayed_actions)
    delayed_actions.emplace_back(std::make_pair(ei, std::move(cs)));
}
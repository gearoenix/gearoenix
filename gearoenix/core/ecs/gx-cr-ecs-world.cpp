#include "gx-cr-ecs-world.hpp"

gearoenix::core::ecs::Entity::id_t gearoenix::core::ecs::World::create_entity_with_builder(Entity::Builder&& b) noexcept
{
    Archetype::id_t archetype_id;
    archetype_id.reserve(b.components.size());
    for (const auto& c : b.components) {
        archetype_id.push_back(c.first);
    }
    auto search = archetypes_map.find(archetype_id);
    if (archetypes_map.end() == search) {
        bool is_ok;
        std::tie(search, is_ok) = archetypes_map.emplace(archetype_id, archetypes.size());
        if (!is_ok)
            GX_LOG_F("Insertion in archetype map was not successful")
        archetypes.push_back(Archetype(b.components));
    }
    const auto ai = search->second;
    const auto index = archetypes[ai].allocate_entity(b.id, b.components);
    entities.emplace(b.id, Entity(ai, index));
    return b.id;
}

gearoenix::core::ecs::Entity::id_t gearoenix::core::ecs::World::delayed_create_entity_with_builder(Entity::Builder&& b) noexcept
{
    const auto id = b.id;
    GX_GUARD_LOCK(delayed_actions)
    delayed_actions.emplace_back(std::move(b));
    return id;
}

void gearoenix::core::ecs::World::remove_entity(const Entity::id_t id) noexcept
{
    auto search = entities.find(id);
#ifdef GX_DEBUG_MODE
    if (entities.end() == search) {
        GX_LOG_E("Deleting an already deleted entity: " << id)
        return;
    }
#endif
    auto& e = search->second;
    const auto r = archetypes[e.archetype].remove_entity(e.index_in_archetype);
    entities.erase(search);
    if (!r.has_value())
        return;
    auto [mid, mi] = *r;
    search = entities.find(mid);
#ifdef GX_DEBUG_MODE
    if (entities.end() == search)
        GX_LOG_F("Can not found the moved entity: " << mid)
#endif
    auto& me = search->second;
    me.index_in_archetype = mi;
}

void gearoenix::core::ecs::World::delayed_remove_entity(const Entity::id_t id) noexcept
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
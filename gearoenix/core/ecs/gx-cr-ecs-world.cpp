#include "gx-cr-ecs-world.hpp"

gearoenix::core::ecs::Entity::id_t gearoenix::core::ecs::World::create_entity_with_builder(Entity::Builder&& b) noexcept
{
    const auto ai = get_archetype_index(b.components);
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
    auto r = archetypes[e.archetype].remove_entity(e.index_in_archetype);
    entities.erase(search);
    update_entity(std::move(r));
}

void gearoenix::core::ecs::World::delayed_remove_entity(const Entity::id_t id) noexcept
{
    GX_GUARD_LOCK(delayed_actions)
    delayed_actions.emplace_back(id);
}

void gearoenix::core::ecs::World::add_components_map(const Entity::id_t id, Entity::Builder::components_t&& c) noexcept
{
    auto search = entities.find(id);
#ifdef GX_DEBUG_MODE
    if (entities.end() == search)
        GX_LOG_F("Can not found the moved entity: " << id)
#endif
    auto& entity = search->second;
    const auto old_archetype_index = entity.archetype;
    const auto old_index_in_archetype = entity.index_in_archetype;
    archetypes[old_archetype_index].move_out_entity(old_index_in_archetype, c);
    Entity::Builder::sort(c);
    const auto new_archetype_index = get_archetype_index(c);
    entity.archetype = new_archetype_index;
    entity.index_in_archetype = archetypes[new_archetype_index].allocate_entity(id, c);
    auto r = archetypes[old_archetype_index].move_from_back(old_index_in_archetype);
    update_entity(std::move(r));
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

std::size_t gearoenix::core::ecs::World::get_archetype_index(const Entity::Builder::components_t& cs) noexcept
{
    Archetype::id_t archetype_id;
    archetype_id.reserve(cs.size());
    for (const auto& c : cs) {
        archetype_id.push_back(c.first);
    }
    auto search = archetypes_map.find(archetype_id);
    if (archetypes_map.end() == search) {
        bool is_ok;
        std::tie(search, is_ok) = archetypes_map.emplace(archetype_id, archetypes.size());
        if (!is_ok)
            GX_LOG_F("Insertion in archetype map was not successful")
        archetypes.push_back(Archetype(cs));
    }
    return search->second;
}

void gearoenix::core::ecs::World::update_entity(std::optional<std::pair<Entity::id_t, std::size_t>>&& r) noexcept
{
    if (!r.has_value())
        return;
    auto [id, i] = *r;
    auto search = entities.find(id);
#ifdef GX_DEBUG_MODE
    if (entities.end() == search)
        GX_LOG_F("Can not found the moved entity: " << id)
#endif
    auto& e = search->second;
    e.index_in_archetype = i;
}

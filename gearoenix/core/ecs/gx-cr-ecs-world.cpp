#include "gx-cr-ecs-world.hpp"

gearoenix::core::ecs::Entity::id_t gearoenix::core::ecs::World::create_entity(Entity::Builder&& b) noexcept
{
    Archetype::id_t archetype_id;
    std::vector<std::vector<std::uint8_t>> cs;
    cs.reserve(b.components.size());
    for (auto& [t, c] : b.components) {
        archetype_id.emplace(t);
        cs.push_back(std::move(c));
    }
    GX_GUARD_LOCK(this)
    auto search = archetypes.find(archetype_id);
    if (archetypes.end() == search) {
        search = archetypes.emplace(archetype_id, Archetype(archetype_id, cs)).first;
    }
    auto& archetype = search->second;
    const auto index = archetype.allocate_entity(b.id, cs);
    entities.emplace(b.id, Entity(archetype_id, index));
    return b.id;
}

gearoenix::core::ecs::Entity::id_t gearoenix::core::ecs::World::delayed_create_entity(Entity::Builder&& b) noexcept
{
    const auto id = b.id;
    GX_GUARD_LOCK(delayed_actions)
    delayed_actions.emplace_back(std::move(b));
    return id;
}

void gearoenix::core::ecs::World::delete_entity(const id_t id) noexcept
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

void gearoenix::core::ecs::World::delayed_delete_entity(const id_t id) noexcept
{
    GX_GUARD_LOCK(delayed_actions)
    delayed_actions.emplace_back(id);
}

void gearoenix::core::ecs::World::add_components(
    const id_t ei,
    const std::map<std::type_index, std::vector<std::uint8_t>>&) noexcept
{
    GX_UNIMPLEMENTED
}

void gearoenix::core::ecs::World::delayed_add_components(
    const id_t ei,
    std::map<std::type_index, std::vector<std::uint8_t>> cs) noexcept
{
    GX_GUARD_LOCK(delayed_actions)
    delayed_actions.emplace_back(std::make_pair(ei, std::move(cs)));
}

void gearoenix::core::ecs::World::remove_components(id_t, const std::type_index*, std::size_t) noexcept
{
    GX_UNIMPLEMENTED
}
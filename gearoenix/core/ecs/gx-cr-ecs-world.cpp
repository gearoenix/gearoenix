#include "gx-cr-ecs-world.hpp"

void gearoenix::core::ecs::World::create_entity_with_builder(EntityBuilder&& b) noexcept
{
    const auto ai = get_archetype_index(b.components);
    const auto index = archetypes[ai].allocate_entity(b.id, b.components);
    entities.emplace(b.id, Entity(ai, index, b.name));
    if (b.name.has_value()) {
        if (name_to_entity_id.contains(*b.name))
            GX_LOG_F("Entity with name `" << *b.name << "' is already in the world.");
        name_to_entity_id.emplace(std::move(*b.name), b.id);
    }
}

void gearoenix::core::ecs::World::delayed_create_entity_with_builder(EntityBuilder&& b) noexcept
{
    std::lock_guard<std::mutex> _lg(delayed_actions_lock);
    delayed_actions.emplace_back(std::move(b));
}

void gearoenix::core::ecs::World::remove_entity(const Entity::id_t id) noexcept
{
    auto search = entities.find(id);
#ifdef GX_DEBUG_MODE
    if (entities.end() == search) {
        GX_LOG_E("Deleting an already deleted entity: " << id);
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
    std::lock_guard<std::mutex> _lg(delayed_actions_lock);
    delayed_actions.emplace_back(id);
}

void gearoenix::core::ecs::World::add_components_map(const Entity::id_t id, EntityBuilder::components_t&& c) noexcept
{
    if (c.empty())
        return;
    auto* const entity = get_entity(id);
    if (nullptr == entity) {
        GX_LOG_E("Entity: " << id << " does not exist.");
        return;
    }
    const auto old_archetype_index = entity->archetype;
    const auto old_index_in_archetype = entity->index_in_archetype;
    archetypes[old_archetype_index].move_out_entity(old_index_in_archetype, c);
    const auto new_archetype_index = get_archetype_index(c);
    entity->archetype = new_archetype_index;
    entity->index_in_archetype = archetypes[new_archetype_index].allocate_entity(id, c);
    auto r = archetypes[old_archetype_index].move_from_back(old_index_in_archetype);
    update_entity(std::move(r));
}

void gearoenix::core::ecs::World::delayed_add_components_map(const Entity::id_t ei, EntityBuilder::components_t&& cs) noexcept
{
    std::lock_guard<std::mutex> _lg(delayed_actions_lock);
    delayed_actions.emplace_back(std::make_pair(ei, std::move(cs)));
}

void gearoenix::core::ecs::World::remove_components_list(
    const Entity::id_t id, const std::type_index* const ts, const std::size_t sz) noexcept
{
    if (0 == sz) {
        GX_LOG_E("At least one component must be selected to remove. Entity: " << id);
        return;
    }
    auto* const entity = get_entity(id);
    if (nullptr == entity) {
        GX_LOG_E("Entity " << id << " does not exist.");
        return;
    }
    const auto old_archetype_index = entity->archetype;
    const auto old_index_in_archetype = entity->index_in_archetype;
    EntityBuilder::components_t cs;
    cs.reserve(archetypes[old_archetype_index].components_indices.size());
    archetypes[old_archetype_index].move_out_entity(old_index_in_archetype, cs);
    for (std::size_t ti = 0; ti < sz; ++ti) {
        cs.erase(ts[ti]);
    }
    const auto new_archetype_index = get_archetype_index(cs);
    entity->archetype = new_archetype_index;
    entity->index_in_archetype = archetypes[new_archetype_index].allocate_entity(id, cs);
    auto r = archetypes[old_archetype_index].move_from_back(old_index_in_archetype);
    update_entity(std::move(r));
}

void gearoenix::core::ecs::World::delayed_remove_components_list(
    const Entity::id_t ei,
    std::vector<std::type_index>&& cs) noexcept
{
    std::lock_guard<std::mutex> _lg(delayed_actions_lock);
    delayed_actions.emplace_back(std::make_pair(ei, std::move(cs)));
}

std::size_t gearoenix::core::ecs::World::get_archetype_index(const EntityBuilder::components_t& cs) noexcept
{
    Archetype::id_t archetype_id;
    archetype_id.reserve(cs.size());
    for (const auto& c : cs) {
        archetype_id.emplace(c.first);
    }
    auto search = archetypes_map.find(archetype_id);
    if (archetypes_map.end() == search) {
        bool is_ok;
        std::tie(search, is_ok) = archetypes_map.emplace(archetype_id, archetypes.size());
        if (!is_ok)
            GX_LOG_F("Insertion in archetype map was not successful");
        archetypes.push_back(Archetype(cs));
    }
    return search->second;
}

gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const Entity::id_t id) noexcept
{
    auto search = entities.find(id);
    if (entities.end() == search)
        return nullptr;
    return &search->second;
}

const gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const Entity::id_t id) const noexcept
{
    const auto search = entities.find(id);
    if (entities.end() == search)
        return nullptr;
    return &search->second;
}

void gearoenix::core::ecs::World::update() noexcept
{
    decltype(delayed_actions) actions;
    {
        std::lock_guard<std::mutex> _lg(delayed_actions_lock);
        std::swap(actions, delayed_actions);
    }
    for (auto& action : actions) {
        switch (action.index()) {
        case 0:
            create_entity_with_builder(std::get<0>(std::move(action)));
            break;
        case 1:
            remove_entity(std::get<1>(std::move(action)));
            break;
        case 2: {
            auto&& [id, cs] = std::get<2>(std::move(action));
            add_components_map(id, std::move(cs));
            break;
        }
        case 3: {
            auto&& [id, ct] = std::get<3>(std::move(action));
            remove_components_list(id, ct.data(), ct.size());
            break;
        }
        default:
            GX_UNEXPECTED;
        }
    }
}

void gearoenix::core::ecs::World::update_entity(std::optional<std::pair<Entity::id_t, std::size_t>>&& r) noexcept
{
    if (!r.has_value())
        return;
    const auto [id, i] = *r;
    get_entity(id)->index_in_archetype = i;
}

std::shared_ptr<gearoenix::core::ecs::EntitySharedBuilder> gearoenix::core::ecs::World::create_shared_builder() noexcept
{
    return std::shared_ptr<EntitySharedBuilder>(new EntitySharedBuilder(this));
}

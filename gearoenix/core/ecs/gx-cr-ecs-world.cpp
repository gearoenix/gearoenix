#include "gx-cr-ecs-world.hpp"

void gearoenix::core::ecs::World::create_entity_with_builder(EntityBuilder&& b) noexcept
{
    auto* const a = get_archetype(b.components);
    auto* const cs = a->allocate_entity(b.id, b.components);
    entities.emplace(b.id, Entity(a, cs, std::string(b.name)));
    if (name_to_entity_id.contains(b.name))
        GX_LOG_F("Entity with name `" << b.name << "' is already in the world.");
    name_to_entity_id.emplace(std::move(b.name), b.id);
}

void gearoenix::core::ecs::World::delayed_create_entity_with_builder(EntityBuilder&& b) noexcept
{
    const std::lock_guard<std::mutex> _lg(delayed_actions_lock);
    delayed_actions.emplace_back(std::move(b));
}

void gearoenix::core::ecs::World::remove_entity(const entity_id_t id) noexcept
{
    auto search = entities.find(id);
#ifdef GX_DEBUG_MODE
    if (entities.end() == search) {
        GX_LOG_E("Deleting an already deleted entity: " << id);
        return;
    }
#endif
    auto& e = search->second;
    e.archetype->remove_entity(e.components);
    name_to_entity_id.erase(e.name);
    entities.erase(search);
}

void gearoenix::core::ecs::World::delayed_remove_entity(const entity_id_t id) noexcept
{
    const std::lock_guard<std::mutex> _lg(delayed_actions_lock);
    delayed_actions.emplace_back(id);
}

void gearoenix::core::ecs::World::add_components_map(const entity_id_t id, EntityBuilder::components_t&& c) noexcept
{
    if (c.empty())
        return;
    auto* const entity = get_entity(id);
    if (nullptr == entity) {
        GX_LOG_E("Entity: " << id << " does not exist.");
        return;
    }
    const auto old_archetype = entity->archetype;
    const auto old_components = entity->components;
    old_archetype->move_out_entity(old_components, c);
    const auto new_archetype = get_archetype(c);
    entity->archetype = new_archetype;
    entity->components = new_archetype->allocate_entity(id, c);
}

void gearoenix::core::ecs::World::delayed_add_components_map(const entity_id_t ei, EntityBuilder::components_t&& cs) noexcept
{
    const std::lock_guard<std::mutex> _lg(delayed_actions_lock);
    delayed_actions.emplace_back(std::make_pair(ei, std::move(cs)));
}

void gearoenix::core::ecs::World::remove_components_list(
    const entity_id_t id, const std::type_index* const ts, const std::size_t sz) noexcept
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
    auto* const old_archetype = entity->archetype;
    auto* const old_components = entity->components;
    EntityBuilder::components_t cs;
    cs.reserve(old_archetype->components_indices.size());
    old_archetype->move_out_entity(old_components, cs);
    for (std::size_t ti = 0; ti < sz; ++ti) {
        cs.erase(ts[ti]);
    }
    auto* const new_archetype = get_archetype(cs);
    entity->archetype = new_archetype;
    entity->components = new_archetype->allocate_entity(id, cs);
}

void gearoenix::core::ecs::World::delayed_remove_components_list(
    const entity_id_t ei,
    std::vector<std::type_index>&& cs) noexcept
{
    const std::lock_guard<std::mutex> _lg(delayed_actions_lock);
    delayed_actions.emplace_back(std::make_pair(ei, std::move(cs)));
}

gearoenix::core::ecs::Archetype* gearoenix::core::ecs::World::get_archetype(const EntityBuilder::components_t& cs) noexcept
{
    Archetype::id_t archetype_id;
    archetype_id.reserve(cs.size());
    for (const auto& c : cs) {
        archetype_id.emplace(c.first);
    }
    auto search = archetypes.find(archetype_id);
    if (archetypes.end() == search) {
        bool is_ok = false;
        std::tie(search, is_ok) = archetypes.emplace(archetype_id, std::unique_ptr<Archetype>(new Archetype(cs)));
        if (!is_ok)
            GX_LOG_F("Insertion in archetype map was not successful");
    }
    return search->second.get();
}

gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const entity_id_t id) noexcept
{
    auto search = entities.find(id);
    if (entities.end() == search)
        return nullptr;
    return &search->second;
}

const gearoenix::core::ecs::Entity* gearoenix::core::ecs::World::get_entity(const entity_id_t id) const noexcept
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
        const std::lock_guard<std::mutex> _lg(delayed_actions_lock);
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

std::shared_ptr<gearoenix::core::ecs::EntitySharedBuilder> gearoenix::core::ecs::World::create_shared_builder(std::string&& name, sync::EndCaller&& end_caller) noexcept
{
    return std::shared_ptr<EntitySharedBuilder>(new EntitySharedBuilder(this, std::move(name), std::move(end_caller)));
}

#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-world.hpp"

gearoenix::core::ecs::Entity::Entity(
    Archetype*const archetype,
    unsigned char*const components,
    std::optional<std::string> name) noexcept
    : archetype(archetype)
    , components(components)
    , name(std::move(name))
{
}

gearoenix::core::ecs::Entity::Entity(Entity&& o) noexcept
    : archetype(o.archetype)
    , components(o.components)
    , name(std::move(o.name))
{
}

std::atomic<gearoenix::core::ecs::entity_id_t> gearoenix::core::ecs::Entity::last_id(1028);

gearoenix::core::ecs::EntityBuilder::EntityBuilder(const entity_id_t id, sync::EndCaller&& end_caller) noexcept
    : id(id)
    , end_caller(std::move(end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(sync::EndCaller&& end_caller) noexcept
    : id(++Entity::last_id)
    , end_caller(std::move(end_caller))
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(EntityBuilder&& o) noexcept
    : id(o.id)
    , components(std::move(o.components))
    , end_caller(std::move(o.end_caller))
{
}

const gearoenix::core::ecs::Component* gearoenix::core::ecs::EntityBuilder::get_component(const std::type_index component_type) const noexcept
{
    const auto search = components.find(component_type);
    if (components.end() == search)
        return nullptr;
    return search->second.get();
}

gearoenix::core::ecs::Component* gearoenix::core::ecs::EntityBuilder::get_component(const std::type_index component_type) noexcept
{
    auto search = components.find(component_type);
    if (components.end() == search)
        return nullptr;
    return search->second.get();
}

gearoenix::core::ecs::EntitySharedBuilder::EntitySharedBuilder(World* const world, sync::EndCaller&& end_caller) noexcept
    : world(world)
    , builder(std::move(end_caller))
{
}

gearoenix::core::ecs::EntitySharedBuilder::~EntitySharedBuilder() noexcept
{
    world->delayed_create_entity_with_builder(std::move(builder));
}

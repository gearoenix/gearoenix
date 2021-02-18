#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-world.hpp"

gearoenix::core::ecs::Entity::Entity(std::size_t archetype, const std::size_t index_in_archetype) noexcept
    : archetype(archetype)
    , index_in_archetype(index_in_archetype)
{
}

gearoenix::core::ecs::Entity::Entity(Entity&& o) noexcept
    : archetype(o.archetype)
    , index_in_archetype(o.index_in_archetype)
{
}

std::atomic<gearoenix::core::ecs::Entity::id_t> gearoenix::core::ecs::Entity::last_id(1028);

gearoenix::core::ecs::EntityBuilder::EntityBuilder(const Entity::id_t id) noexcept
    : id(id)
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder() noexcept
    : id(++Entity::last_id)
{
}

gearoenix::core::ecs::EntityBuilder::EntityBuilder(EntityBuilder&& o) noexcept
    : id(o.id)
    , components(std::move(o.components))
{
}

void gearoenix::core::ecs::EntityBuilder::sort(components_t& cs) noexcept
{
    std::sort(cs.begin(), cs.end(), component_less);
}

gearoenix::core::ecs::EntitySharedBuilder::EntitySharedBuilder(World* const world) noexcept
    : world(world)
{
}

gearoenix::core::ecs::EntitySharedBuilder::~EntitySharedBuilder() noexcept
{
    world->delayed_create_entity_with_builder(std::move(builder));
}

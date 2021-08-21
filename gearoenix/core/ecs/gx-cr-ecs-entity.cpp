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

const void* gearoenix::core::ecs::EntityBuilder::get_component(const std::type_index component_type) const noexcept
{
    const auto search = std::lower_bound(
        components.begin(), components.end(), component_type,
        [](const component_t& rhs, const std::type_index ti) noexcept -> bool {
            return rhs.first < ti;
        });
    if (components.end() == search)
        return nullptr;
    return search->second.data();
}

void* gearoenix::core::ecs::EntityBuilder::get_component(const std::type_index component_type) noexcept
{
    const auto search = std::lower_bound(
        components.begin(), components.end(), component_type,
        [](const component_t& rhs, const std::type_index ti) noexcept -> bool {
            return rhs.first < ti;
        });
    if (components.end() == search)
        return nullptr;
    return search->second.data();
}

void gearoenix::core::ecs::EntityBuilder::sort(components_t& cs) noexcept
{
    std::sort(cs.begin(), cs.end(), [](const component_t& l, const component_t& r) noexcept -> bool {
        return l.first < r.first;
    });
}

gearoenix::core::ecs::EntitySharedBuilder::EntitySharedBuilder(World* const world) noexcept
    : world(world)
{
}

gearoenix::core::ecs::EntitySharedBuilder::~EntitySharedBuilder() noexcept
{
    world->delayed_create_entity_with_builder(std::move(builder));
}

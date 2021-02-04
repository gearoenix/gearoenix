#include "gx-cr-ecs-entity.hpp"

std::atomic<gearoenix::core::ecs::Entity::id_t> gearoenix::core::ecs::Entity::last_id(0);

gearoenix::core::ecs::Entity::Builder::Builder(const id_t id) noexcept
    : id(id)
{
}

gearoenix::core::ecs::Entity::Builder::Builder() noexcept
    : id(++last_id)
{
}

gearoenix::core::ecs::Entity::Builder::Builder(Builder&& o) noexcept
    : id(o.id)
    , components(std::move(o.components))
{
}

void gearoenix::core::ecs::Entity::Builder::sort(components_t& cs) noexcept
{
    std::sort(cs.begin(), cs.end(), component_less);
}

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

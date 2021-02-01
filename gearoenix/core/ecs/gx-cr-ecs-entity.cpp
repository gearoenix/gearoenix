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

gearoenix::core::ecs::Entity::Entity(archetype_id_t archetype, const std::size_t index_in_archetype) noexcept
    : archetype(std::move(archetype))
    , index_in_archetype(index_in_archetype)
{
}

gearoenix::core::ecs::Entity::Entity(Entity&& o) noexcept
    : archetype(std::move(o.archetype))
    , index_in_archetype(o.index_in_archetype)
{
}

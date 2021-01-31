#include "gx-cr-ecs-entity.hpp"

std::atomic<gearoenix::core::ecs::Entity::id_t> gearoenix::core::ecs::Entity::last_id(0);

gearoenix::core::ecs::Entity::Builder::Builder() noexcept
    : id(++last_id)
{
}

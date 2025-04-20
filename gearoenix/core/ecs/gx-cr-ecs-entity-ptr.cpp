#include "gx-cr-ecs-entity-ptr.hpp"
#include "gx-cr-ecs-world.hpp"

gearoenix::core::ecs::EntityPtr::~EntityPtr()
{
    clear();
}

gearoenix::core::ecs::EntityPtr::EntityPtr(const EntityPtr&) = default;

gearoenix::core::ecs::EntityPtr::EntityPtr(EntityPtr&&) noexcept = default;

gearoenix::core::ecs::EntityPtr& gearoenix::core::ecs::EntityPtr::operator=(const EntityPtr& o)
{
    clear();
    ptr = o.ptr;
    return *this;
}

gearoenix::core::ecs::EntityPtr& gearoenix::core::ecs::EntityPtr::operator=(EntityPtr&& o) noexcept
{
    clear();
    ptr = std::move(o.ptr);
    return *this;
}

void gearoenix::core::ecs::EntityPtr::clear()
{
    if (ptr.use_count() == 1) {
        World::get().delayed_delete_entity(std::move(ptr));
    }
}

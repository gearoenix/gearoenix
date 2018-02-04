#include "cr-ev-mv-movement.hpp"

gearoenix::core::event::movement::Movement::Movement(MovementType t)
    : Event(EventType::MOVEMENT)
    , movement_type(t)
{
}

gearoenix::core::event::movement::Movement::~Movement() {}

gearoenix::core::event::movement::Movement::MovementType gearoenix::core::event::movement::Movement::get_type() const
{
    return movement_type;
}

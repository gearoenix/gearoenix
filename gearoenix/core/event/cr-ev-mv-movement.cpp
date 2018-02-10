#include "cr-ev-mv-movement.hpp"
#include "../../system/sys-log.hpp"
#include "../cr-build-configuration.hpp"
#include "cr-ev-mv-mouse.hpp"

gearoenix::core::event::movement::Movement::Movement(MovementType t)
    : Event(From::MOVEMENT)
    , movement_type(t)
{
}

gearoenix::core::event::movement::Movement::~Movement() {}

gearoenix::core::event::movement::Movement::MovementType gearoenix::core::event::movement::Movement::get_type() const
{
    return movement_type;
}

const gearoenix::core::event::movement::Mouse& gearoenix::core::event::movement::Movement::to_mouse() const
{
#ifdef DEBUG_MODE
    if (movement_type != MovementType::MOUSE)
        UNEXPECTED;
#endif
    return *static_cast<const Mouse*>(this);
}

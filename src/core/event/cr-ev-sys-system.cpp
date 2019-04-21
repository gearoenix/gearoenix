#include "cr-ev-sys-system.hpp"

gearoenix::core::event::system::System::System(Action::Type t)
    : Event(Event::From::SYSTEM)
    , action_type(t)
{
}

gearoenix::core::event::system::System::~System() {}

gearoenix::core::event::system::System::Action::Type gearoenix::core::event::system::System::get_action() const
{
    return action_type;
}

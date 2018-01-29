#include "cr-ev-event.hpp"
#include "../../system/sys-log.hpp"

gearoenix::core::event::Event::Event(EventType t)
    : event_type(t)
{
}

gearoenix::core::event::Event::~Event()
{
}

const gearoenix::core::event::button::Button* gearoenix::core::event::Event::to_button() const
{
    if (BUTTON == event_type)
        return reinterpret_cast<const button::Button*>(this);
    UNEXPECTED;
}

const gearoenix::core::event::movement::Movement* gearoenix::core::event::Event::to_movement() const
{
    if (MOVEMENT == event_type)
        return reinterpret_cast<const movement::Movement*>(this);
    UNEXPECTED;
}

const gearoenix::core::event::WindowResize* gearoenix::core::event::Event::to_window_resize() const
{
    if (WINDOW_RESIZE == event_type)
        return reinterpret_cast<const event::WindowResize*>(this);
    UNEXPECTED;
}

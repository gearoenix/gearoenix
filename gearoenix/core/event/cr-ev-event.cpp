#include "cr-ev-event.hpp"
#include "../../system/sys-log.hpp"

gearoenix::core::event::Event::Event(EventType t)
    : event_type(t)
{
}

gearoenix::core::event::Event::~Event()
{
}

gearoenix::core::event::Event::EventType gearoenix::core::event::Event::get_type() const
{
	return event_type;
}

const gearoenix::core::event::button::Button& gearoenix::core::event::Event::to_button() const
{
#ifdef DEBUG_MODE
    if (BUTTON != event_type) UNEXPECTED;
#endif
    return *reinterpret_cast<const button::Button*>(this);
}

const gearoenix::core::event::movement::Movement& gearoenix::core::event::Event::to_movement() const
{
#ifdef DEBUG_MODE
    if (MOVEMENT != event_type)
		UNEXPECTED;
#endif
    return *reinterpret_cast<const movement::Movement*>(this);
}

const gearoenix::core::event::WindowResize& gearoenix::core::event::Event::to_window_resize() const
{
#ifdef DEBUG_MODE
    if (WINDOW_RESIZE != event_type)
		UNEXPECTED;
#endif
	return *reinterpret_cast<const event::WindowResize*>(this);
}

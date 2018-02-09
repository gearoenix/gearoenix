#include "cr-ev-event.hpp"
#include "../../system/sys-log.hpp"
#include "cr-ev-bt-button.hpp"
#include "cr-ev-mv-movement.hpp"
#include "cr-ev-sys-system.hpp"
#include "cr-ev-ui-ui.hpp"
#include "cr-ev-window-resize.hpp"

gearoenix::core::event::Event::Event(From::Type t)
    : event_type(t)
{
}

gearoenix::core::event::Event::~Event()
{
}

gearoenix::core::event::Event::From::Type gearoenix::core::event::Event::get_type() const
{
    return event_type;
}

const gearoenix::core::event::button::Button& gearoenix::core::event::Event::to_button() const
{
#ifdef DEBUG_MODE
    if (From::BUTTON != event_type)
        UNEXPECTED;
#endif
    return *static_cast<const button::Button*>(this);
}

const gearoenix::core::event::movement::Movement& gearoenix::core::event::Event::to_movement() const
{
#ifdef DEBUG_MODE
    if (From::MOVEMENT != event_type)
        UNEXPECTED;
#endif
    return *static_cast<const movement::Movement*>(this);
}

const gearoenix::core::event::system::System& gearoenix::core::event::Event::to_system() const
{
#ifdef DEBUG_MODE
    if (From::SYSTEM != event_type)
        UNEXPECTED;
#endif
    return *static_cast<const system::System*>(this);
}

const gearoenix::core::event::ui::Ui& gearoenix::core::event::Event::to_ui() const
{
#ifdef DEBUG_MODE
    if (From::UI != event_type)
        UNEXPECTED;
#endif
    return *static_cast<const ui::Ui*>(this);
}

const gearoenix::core::event::WindowResize& gearoenix::core::event::Event::to_window_resize() const
{
#ifdef DEBUG_MODE
    if (From::WINDOW_RESIZE != event_type)
        UNEXPECTED;
#endif
    return *static_cast<const event::WindowResize*>(this);
}

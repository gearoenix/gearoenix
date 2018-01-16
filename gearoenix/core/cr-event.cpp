#include "cr-event.hpp"

gearoenix::core::event::Event::~Event()
{
}

const gearoenix::core::event::WindowResize* gearoenix::core::event::Event::to_window_resize() const
{
    if (WINDOW_RESIZE == get_type())
        return reinterpret_cast<const event::WindowResize*>(this);
    return nullptr;
}

gearoenix::core::event::WindowResize::WindowResize(Real pre_width, Real pre_height, Real cur_width, Real cur_height)
    : pre_width(pre_width)
    , pre_height(pre_height)
    , cur_width(cur_width)
    , cur_height(cur_height)
{
}

gearoenix::core::event::Event::Type gearoenix::core::event::WindowResize::get_type() const
{
    return WINDOW_RESIZE;
}

gearoenix::core::Real gearoenix::core::event::WindowResize::get_previous_width() const
{
    return pre_width;
}

gearoenix::core::Real gearoenix::core::event::WindowResize::get_previous_height() const
{
    return pre_height;
}

gearoenix::core::Real gearoenix::core::event::WindowResize::get_current_width() const
{
    return cur_width;
}

gearoenix::core::Real gearoenix::core::event::WindowResize::get_current_height() const
{
    return cur_height;
}

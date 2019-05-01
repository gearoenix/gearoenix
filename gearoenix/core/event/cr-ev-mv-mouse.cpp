#include "cr-ev-mv-mouse.hpp"

gearoenix::core::event::movement::Mouse::Mouse(core::Real x, core::Real y, core::Real pre_x, core::Real pre_y)
    : Movement(MovementType::MOUSE)
    , x(x)
    , y(y)
    , pre_x(pre_x)
    , pre_y(pre_y)
    , dx(x - pre_x)
    , dy(y - pre_y)
{
}

gearoenix::core::Real gearoenix::core::event::movement::Mouse::get_x() const
{
    return x;
}

gearoenix::core::Real gearoenix::core::event::movement::Mouse::get_y() const
{
    return y;
}

gearoenix::core::Real gearoenix::core::event::movement::Mouse::get_dx() const { return dx; }

gearoenix::core::Real gearoenix::core::event::movement::Mouse::get_dy() const { return dy; }

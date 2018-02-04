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

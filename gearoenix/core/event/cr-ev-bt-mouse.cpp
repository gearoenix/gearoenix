#include "cr-ev-bt-button.hpp"
#include "cr-ev-bt-mouse.hpp"

gearoenix::core::event::button::Mouse::Mouse(KeyType k, ActionType a, core::Real x, core::Real y)
	: Button(ButtonType::MOUSE, k, a)
	, x(x)
	, y(y)
{
}

gearoenix::core::event::button::Mouse::~Mouse()
{
}

gearoenix::core::Real gearoenix::core::event::button::Mouse::get_x() const
{
	return x;
}

gearoenix::core::Real gearoenix::core::event::button::Mouse::get_y() const
{
	return y;
}

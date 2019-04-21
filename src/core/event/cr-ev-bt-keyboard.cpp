#include "cr-ev-bt-keyboard.hpp"

gearoenix::core::event::button::Keyboard::Keyboard(KeyType k, ActionType a)
    : Button(ButtonType::KEYBOARD, k, a)
{
}

gearoenix::core::event::button::Keyboard::~Keyboard()
{
}
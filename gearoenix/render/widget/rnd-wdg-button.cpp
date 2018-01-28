#include "rnd-wdg-button.hpp"

gearoenix::render::widget::Button::Button(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Widget(f, e, c)
{
}

gearoenix::render::widget::Button::~Button() {}

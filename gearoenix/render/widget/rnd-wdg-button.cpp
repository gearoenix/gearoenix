#include "rnd-wdg-button.hpp"
#include "../../core/event/cr-ev-bt-button.hpp"
#include "../../physics/animation/phs-anm-once.hpp"

gearoenix::render::widget::Button::Button(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Widget(f, e, c)
{
}

gearoenix::render::widget::Button::~Button() {}

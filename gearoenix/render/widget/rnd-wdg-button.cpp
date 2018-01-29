#include "rnd-wdg-button.hpp"
#include "../../physics/animation/phs-anm-once.hpp"

gearoenix::render::widget::Button::Button(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Widget(f, e, c)
{
    physics::animation::Once([](core::Real) -> void {}, std::chrono::milliseconds(1));
}

gearoenix::render::widget::Button::~Button() {}

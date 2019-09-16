#include "rnd-wdg-button.hpp"
#include "../../core/event/cr-ev-button.hpp"
#include "../../physics/animation/phs-anm-animation.hpp"
#include "../../physics/phs-engine.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"

gearoenix::render::widget::Button::Button(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Button, f, e, c)
{
}

gearoenix::render::widget::Button::Button(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Button, e, c)
{
}

gearoenix::render::widget::Button::~Button() noexcept
{
    animation->set_activity(false);
}

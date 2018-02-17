#include "rnd-wdg-text.hpp"

gearoenix::render::widget::Text::Text(system::stream::Stream* s, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Widget(s, e, c)
{
}

gearoenix::render::widget::Text::~Text() {}

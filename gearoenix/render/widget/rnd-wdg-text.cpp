#include "rnd-wdg-text.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../font/rnd-fnt-font.hpp"
#include "../rnd-engine.hpp"

gearoenix::render::widget::Text::Text(system::stream::Stream* s, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Widget(s, e, c)
    , text(s->read_string())
    , align(s->read<Alignment::Type>())
    , fnt(e->get_system_application()->get_asset_manager()->get_font(s->read<core::Id>(), core::EndCaller<font::Font>([c](std::shared_ptr<font::Font>) -> void {})))
{
}

gearoenix::render::widget::Text::~Text() {}

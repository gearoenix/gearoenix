#include "rnd-fnt-font.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "rnd-fnt-2d.hpp"

gearoenix::render::font::Font::Font(core::Id my_id, Shape::Type t)
    : core::asset::Asset(my_id, core::asset::Asset::AssetType::FONT)
    , font_type(t)
{
}

gearoenix::render::font::Font::~Font()
{
}

gearoenix::render::font::Font* gearoenix::render::font::Font::read(core::Id font_id, system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
{
    Shape::Type font_type;
    f->read(font_type);
    switch (font_type) {
    case Shape::D2:
        return new Font2D(font_id, f, e, c);
        break;
    case Shape::D3:
        UNIMPLEMENTED;
        break;
    default:
        UNEXPECTED;
    }
    return nullptr;
}

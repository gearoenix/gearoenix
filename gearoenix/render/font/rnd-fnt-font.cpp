#include "rnd-fnt-font.hpp"
#include "../../system/file/sys-fl-file.hpp"
#include "rnd-fnt-2d.hpp"

gearoenix::render::font::Font::Font(core::Id my_id, Shape::Type t)
    : font_type(t)
    , my_id(my_id)
{
}

gearoenix::render::font::Font::~Font()
{
}

gearoenix::render::font::Font* gearoenix::render::font::Font::read(core::Id font_id, system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
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

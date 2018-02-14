#include "rnd-fnt-font.hpp"
#include "../../system/sys-file.hpp"
#include "rnd-fnt-2d.hpp"

gearoenix::render::font::Font::Font(Shape::Type t)
    : font_type(t)
{
}

gearoenix::render::font::Font::~Font()
{
}

gearoenix::render::font::Font* gearoenix::render::font::Font::read(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
{
    Shape::Type font_type;
    f->read(font_type);
    switch (font_type) {
    case Shape::D2:
        return new Font2D(f, e, c);
        break;
    case Shape::D3:
        UNIMPLEMENTED;
        break;
    default:
        UNEXPECTED;
    }
    return nullptr;
}

#include "rnd-fnt-font.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "rnd-fnt-2d.hpp"

gearoenix::render::font::Font::Font(const core::Id my_id, const Type::Id t)
    : font_type_id(t)
{
}

gearoenix::render::font::Font::~Font()
{
}

gearoenix::render::font::Font* gearoenix::render::font::Font::read(
    const core::Id font_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c)
{
    const Type::Id font_type = f->read<Type::Id>();
    switch (font_type) {
    case Type::D2:
        return new Font2D(font_id, f, e, c);
        break;
    case Type::D3:
        GXUNIMPLEMENTED;
        break;
    default:
        GXUNEXPECTED;
    }
    return nullptr;
}

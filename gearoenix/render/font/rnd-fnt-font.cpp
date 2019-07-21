#include "rnd-fnt-font.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "rnd-fnt-2d.hpp"

gearoenix::render::font::Font::Font(const core::Id my_id, const Type::Id t) noexcept
    : font_type_id(t)
{
}

gearoenix::render::font::Font::~Font() noexcept
{
}

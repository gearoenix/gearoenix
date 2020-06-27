#include "rnd-fnt-font.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "rnd-fnt-2d.hpp"

gearoenix::render::font::Font::Font(const core::Id id, std::string name, const Type::Id t) noexcept
    : core::asset::Asset(id, core::asset::Type::Font, std::move(name))
    , font_type_id(t)
{
}

gearoenix::render::font::Font::~Font() noexcept = default;

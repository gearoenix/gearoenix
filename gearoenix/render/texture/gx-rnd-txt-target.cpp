#include "gx-rnd-txt-target.hpp"
#include "gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-txt-texture-cube.hpp"

gearoenix::render::texture::Target::Target(std::vector<Attachment>&& attachments) noexcept
    : attachments(std::move(attachments))
{
}

gearoenix::render::texture::Target::~Target() noexcept = default;

gearoenix::math::Vec2<std::size_t> gearoenix::render::texture::Target::get_dimension() const noexcept
{
    const auto& t = attachments[0].var;
    const auto& i = t.index() == 0 ? std::get<0>(t).txt->get_info() : std::get<1>(t).txt->get_info();
    return math::Vec2<std::size_t>(i.width, i.height);
}
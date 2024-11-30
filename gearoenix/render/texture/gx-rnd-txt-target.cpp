#include "gx-rnd-txt-target.hpp"
#include "gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-txt-texture-cube.hpp"

gearoenix::render::texture::Target::Target(std::vector<Attachment>&& attachments)
    : attachments(std::move(attachments))
{
}

gearoenix::render::texture::Target::~Target() = default;

gearoenix::math::Vec2<std::uint32_t> gearoenix::render::texture::Target::get_dimension() const
{
    GX_ASSERT_D(!attachments.empty());
    const auto& t = attachments[0].var;
    const auto& i = t.index() == Attachment::ATTACHMENT_2D_VARIANT_INDEX ? std::get<Attachment::ATTACHMENT_2D_VARIANT_INDEX>(t).txt->get_info() : std::get<Attachment::ATTACHMENT_CUBE_VARIANT_INDEX>(t).txt->get_info();
    return { i.get_width(), i.get_height() };
}

float gearoenix::render::texture::Target::get_aspect_ratio() const
{
    const auto d = get_dimension();
    return static_cast<float>(d.x) / static_cast<float>(d.y);
}

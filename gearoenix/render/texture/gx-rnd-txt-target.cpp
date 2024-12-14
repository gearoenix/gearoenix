#include "gx-rnd-txt-target.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-txt-texture-cube.hpp"

gearoenix::render::texture::Target::Target(std::string&& name, std::vector<Attachment>&& attachments)
    : name(std::move(name))
    , attachments(std::move(attachments))
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

void gearoenix::render::texture::Target::write(
    std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const
{
    s->write_fail_debug(name);
    GX_ASSERT(attachments.size() < 255);
    s->write_fail_debug(static_cast<std::uint8_t>(attachments.size()));
    auto ss = std::make_shared<std::vector<std::shared_ptr<platform::stream::Stream>>>();
    core::job::EndCaller e([s = std::move(s), ss, end = std::move(end)] {
        for (const auto& ms : *ss) {
            s->write(*ms);
        }
        (void)end;
    });
    for (const auto& a : attachments) {
        auto ms = std::make_shared<platform::stream::Memory>();
        ss->push_back(ms);
        a.write(std::move(ms), core::job::EndCaller(e));
    }
}

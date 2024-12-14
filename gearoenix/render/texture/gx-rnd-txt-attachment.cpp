#include "gx-rnd-txt-attachment.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::texture::Attachment2D::Attachment2D(std::shared_ptr<Texture2D>&& txt)
    : txt(std::move(txt))
{
}

gearoenix::render::texture::Attachment2D::~Attachment2D() = default;

void gearoenix::render::texture::Attachment2D::write(
    std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const
{
    txt->write(s, end, false);
}

void gearoenix::render::texture::Attachment2D::read(
    std::shared_ptr<platform::stream::Stream>&& s,
    core::job::EndCaller<Attachment2D>&& e)
{
    core::job::send_job_to_pool([s = std::move(s), e = std::move(e)]() mutable {
        engine::Engine::get()->get_texture_manager()->read_gx3d(
            *s,
            core::job::EndCallerShared<Texture>([e = std::move(e)](std::shared_ptr<Texture>&& t) {
                e.set_return(Attachment2D(std::static_pointer_cast<Texture2D>(std::move(t))));
            }));
    });
}

gearoenix::render::texture::AttachmentCube::AttachmentCube(std::shared_ptr<TextureCube>&& txt, const Face face)
    : face(face)
    , txt(std::move(txt))
{
}

gearoenix::render::texture::AttachmentCube::~AttachmentCube() = default;

void gearoenix::render::texture::AttachmentCube::write(
    std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const
{
    s->write_fail_debug(face);
    txt->write(s, end, false);
}

void gearoenix::render::texture::AttachmentCube::read(
    std::shared_ptr<platform::stream::Stream>&& s,
    core::job::EndCaller<AttachmentCube>&& e)
{
    core::job::send_job_to_pool([s = std::move(s), e = std::move(e)]() mutable {
        const auto face = s->read<Face>();
        engine::Engine::get()->get_texture_manager()->read_gx3d(
            *s,
            core::job::EndCallerShared<Texture>([face, e = std::move(e)](std::shared_ptr<Texture>&& t) {
                e.set_return(AttachmentCube(std::static_pointer_cast<TextureCube>(std::move(t)), face));
            }));
    });
}

gearoenix::render::texture::Attachment::Attachment(
    const std::variant<Attachment2D, AttachmentCube>& var,
    const std::uint8_t mipmap_level)
    : mipmap_level(mipmap_level)
    , var(var)
{
}

gearoenix::render::texture::Attachment::~Attachment() = default;

void gearoenix::render::texture::Attachment::write(
    std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const
{
    s->write_fail_debug(mipmap_level);
    s->write_fail_debug(static_cast<std::uint8_t>(var.index()));
    switch (static_cast<decltype(ATTACHMENT_2D_VARIANT_INDEX)>(var.index())) {
    case ATTACHMENT_2D_VARIANT_INDEX: {
        const auto& a = std::get<ATTACHMENT_2D_VARIANT_INDEX>(var);
        a.write(std::move(s), std::move(end));
        break;
    }
    case ATTACHMENT_CUBE_VARIANT_INDEX: {
        const auto& a = std::get<ATTACHMENT_CUBE_VARIANT_INDEX>(var);
        a.write(std::move(s), std::move(end));
        break;
    }
    default:
        GX_UNEXPECTED;
    }
}

void gearoenix::render::texture::Attachment::read(
    std::shared_ptr<platform::stream::Stream>&& s,
    core::job::EndCaller<Attachment>&& end)
{
    core::job::send_job_to_pool([s = std::move(s), e = std::move(end)]() mutable {
        e.set_return(Attachment {});
        s->read(e.get_return().mipmap_level);
        switch (s->read<std::uint8_t>()) {
        case ATTACHMENT_2D_VARIANT_INDEX: {
            Attachment2D::read(std::move(s), core::job::EndCaller<Attachment2D>([e = std::move(e)](Attachment2D&& a) {
                e.get_return().var = std::move(a);
            }));
            break;
        }
        case ATTACHMENT_CUBE_VARIANT_INDEX: {
            AttachmentCube::read(std::move(s), core::job::EndCaller<AttachmentCube>([e = std::move(e)](AttachmentCube&& a) {
                e.get_return().var = std::move(a);
            }));
            break;
        }
        default: {
            GX_UNEXPECTED;
        }
        }
    });
}

void gearoenix::render::texture::Attachment::read(
    std::shared_ptr<platform::stream::Stream>&& s,
    core::job::EndCaller<std::vector<Attachment>>&& end)
{
    core::job::send_job_to_pool([s = std::move(s), end = std::move(end)]() mutable {
        const auto count = s->read<std::uint8_t>();
        end.set_return(std::vector<Attachment>());
        end.get_return().reserve(count);
        for (auto i = decltype(count) { 0 }; i < count; ++i) {
            std::shared_ptr<platform::stream::Stream> ms = std::make_shared<platform::stream::Memory>();
            s->read(*ms);
            read(std::move(ms), core::job::EndCaller<Attachment>([end](Attachment&& a) {
                end.get_return().push_back(std::move(a));
            }));
        }
    });
}

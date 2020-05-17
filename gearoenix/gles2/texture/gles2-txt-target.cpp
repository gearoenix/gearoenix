#include "gles2-txt-target.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../gl/gl-utils.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-2d.hpp"
#include "gles2-txt-cube.hpp"
#include <tuple>

#ifdef GX_DEBUG_GLES2
#define GX_DEBUG_GLES2_TARGET
#endif

gearoenix::gles2::texture::Framebuffer::~Framebuffer() noexcept
{
    if (borrowed) // It means that this is the main render target
        return;
    gl_e->get_function_loader()->load([cf { framebuffer }, cr { depth_buffer }] {
        if (cf != -1)
            gl::Loader::delete_framebuffers(1, reinterpret_cast<const gl::uint*>(&cf));
        if (cr != -1)
            gl::Loader::delete_renderbuffers(1, reinterpret_cast<const gl::uint*>(&cr));
    });
    framebuffer = -1;
    depth_buffer = -1;
}

void gearoenix::gles2::texture::Framebuffer::bind() const noexcept
{
    if (-1 != depth_buffer)
        gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, depth_buffer);
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
}

gearoenix::gles2::texture::Target::Target(
    const core::Id id,
    engine::Engine* const e) noexcept
    : render::texture::Target(id, e)
    , framebuffer(new Framebuffer())
    , gl_e(e)
{
}

gearoenix::gles2::texture::Target::Target(const Target& o) noexcept = default;

void gearoenix::gles2::texture::Target::initialize_texture(
    const render::texture::AttachmentInfo& info,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    clipping_width = info.img_width;
    clipping_height = info.img_height;
    attachments.resize(1);
    auto& a = attachments[0];
    a.img_width = info.img_width;
    a.img_height = info.img_height;
    a.img_depth = info.img_depth;
    a.mipmap_level = info.mipmap_level;
    a.usage = info.usage;
    if (info.txt.has_value()) {
        a.txt = info.txt.value();
        switch (info.texture_info.texture_type) {
        case render::texture::Type::Texture2D: {
            a.var = render::texture::Attachment2D {
                .txt = std::dynamic_pointer_cast<Texture2D>(info.txt.value()),
            };
            break;
        }
        case render::texture::Type::TextureCube: {
#ifdef GX_DEBUG_GLES2_TARGET
            if (info.img_width != info.img_height)
                GXLOGF("This graphic backend does not support un-squared cube maps.")
#endif
            a.var = render::texture::AttachmentCube {
                .txt = std::dynamic_pointer_cast<TextureCube>(info.txt.value()),
                .face = info.face.value(),
            };
            break;
        }
        default:
            GXUNEXPECTED
        }
    } else {
        switch (info.texture_info.texture_type) {
        case render::texture::Type::Texture2D: {
            const auto t = Texture2D::construct(
                core::asset::Manager::create_id(), gl_e, {},
                info.texture_info,
                info.img_width, info.img_height, call);
            a.txt = t;
            a.var = render::texture::Attachment2D {
                .txt = t,
            };
            break;
        }
        case render::texture::Type::TextureCube: {
#ifdef GX_DEBUG_GLES2_TARGET
            if (info.img_width != info.img_height)
                GXLOGF("This graphic backend does not support un-squared cube maps.")
#endif
            const auto t = TextureCube::construct(
                core::asset::Manager::create_id(), gl_e, {},
                info.texture_info,
                info.img_width, call);
            a.txt = t;
            a.var = render::texture::AttachmentCube {
                .txt = t,
                .face = info.face.value(),
            };
            break;
        }
        default:
            GXUNEXPECTED
        }
    }
}

void gearoenix::gles2::texture::Target::state_init() const noexcept
{
#ifdef GX_DEBUG_GLES2_TARGET
    gl::Loader::check_for_error();
#endif
    gl::Loader::clear_color(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

    if (gl_cull_mode.has_value()) {
        gl::Loader::enable(GL_CULL_FACE);
        gl::Loader::cull_face(gl_cull_mode.value());
    } else {
        gl::Loader::disable(GL_CULL_FACE);
    }

    if (gl_blend_mode.has_value()) {
        gl::Loader::enable(GL_BLEND);
        gl::Loader::blend_func(gl_blend_mode.value().first, gl_blend_mode.value().second);
    } else {
        gl::Loader::disable(GL_BLEND);
    }

    if (depth_test_enabled) {
        gl::Loader::enable(GL_DEPTH_TEST);
    } else {
        gl::Loader::disable(GL_DEPTH_TEST);
    }

    if (scissor_test_enabled) {
        gl::Loader::enable(GL_SCISSOR_TEST);
    } else {
        gl::Loader::disable(GL_SCISSOR_TEST);
    }

    if (stencil_test_enabled) {
        gl::Loader::enable(GL_STENCIL_TEST);
    } else {
        gl::Loader::disable(GL_STENCIL_TEST);
    }
#ifdef GX_DEBUG_GLES2_TARGET
    gl::Loader::check_for_error();
#endif
}

void gearoenix::gles2::texture::Target::fetch_current_framebuffer() noexcept
{
    framebuffer = std::make_shared<Framebuffer>();
    framebuffer->borrowed = true;
    framebuffer->gl_e = gl_e;
    gl::Loader::get_integerv(GL_FRAMEBUFFER_BINDING, &(framebuffer->framebuffer));
    gl::Loader::get_integerv(GL_RENDERBUFFER_BINDING, &(framebuffer->depth_buffer));
}

void gearoenix::gles2::texture::Target::generate_framebuffer() noexcept
{
    const auto& attachment = attachments[0];
    const auto& txt = attachment.txt;
    framebuffer->gl_e = gl_e;
    gl::Loader::gen_framebuffers(1, reinterpret_cast<gl::uint*>(&(framebuffer->framebuffer)));
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer);
    gl::Loader::gen_renderbuffers(1, reinterpret_cast<gl::uint*>(&framebuffer->depth_buffer));
    gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, framebuffer->depth_buffer);
    gl::Loader::renderbuffer_storage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, attachment.img_width, attachment.img_height);
    gl::Loader::framebuffer_renderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depth_buffer);
    const auto [glo, glt] = [&] {
        switch (txt->get_texture_type()) {
        case render::texture::Type::Texture2D: {
            const auto* const t = static_cast<const Texture2D*>(txt.get());
            t->bind();
            return std::make_tuple(t->get_texture_object(), gl::enumerated(GL_TEXTURE_2D));
        }
        case render::texture::Type::TextureCube: {
            const auto* const t = static_cast<const TextureCube*>(txt.get());
            t->bind();
            return std::make_tuple(t->get_texture_object(),
                gl::convert(std::get<render::texture::AttachmentCube>(attachment.var).face));
        }
        default:
            GXUNEXPECTED
        }
    }();
    gl::Loader::framebuffer_texture_2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glt, glo, attachment.mipmap_level);
    if (gl::Loader::check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        GXLOGF("Failed to create render target!")
    state_init();
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, 0);
}

gearoenix::gles2::texture::Target::Target(engine::Engine* const e) noexcept
    : render::texture::Target(0, e)
    , gl_e(e)
{
    const auto* const sys_app = e->get_system_application();
    clipping_width = static_cast<unsigned int>(sys_app->get_event_engine()->get_window_width());
    clipping_height = static_cast<unsigned int>(sys_app->get_event_engine()->get_window_height());
    fetch_current_framebuffer();
    state_init();
}

gearoenix::gles2::texture::Target::~Target() noexcept = default;

std::shared_ptr<gearoenix::render::texture::Target> gearoenix::gles2::texture::Target::construct(
    const core::Id id,
    engine::Engine* const e,
    const render::texture::AttachmentInfo& info,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<Target> result(new Target(id, e));
    result->initialize_texture(info, call);
    e->get_function_loader()->load([result, call] {
        result->generate_framebuffer();
    });
    return result;
}

void gearoenix::gles2::texture::Target::bind(const Target* const bound) const noexcept
{
    framebuffer->bind();

    state_init();

    gl::Loader::viewport(
        static_cast<gl::sizei>(clipping_starting_x), static_cast<gl::sizei>(clipping_starting_y),
        static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));
    gl::Loader::scissor(
        static_cast<gl::sizei>(clipping_starting_x), static_cast<gl::sizei>(clipping_starting_y),
        static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));

    clear(bound);
}

void gearoenix::gles2::texture::Target::clear(const Target* const bound) const noexcept
{
    if (write_depth) {
        gl::Loader::depth_mask(GL_TRUE);
    } else {
        gl::Loader::depth_mask(GL_FALSE);
    }

    if (bound == nullptr || bound->framebuffer->framebuffer != framebuffer->framebuffer) {
        gl::Loader::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    } else {
        gl::Loader::clear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

gearoenix::render::texture::Target* gearoenix::gles2::texture::Target::clone() const noexcept
{
    return new Target(*this);
}

#endif

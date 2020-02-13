#include "gles2-txt-target.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-2d.hpp"
#include "gles2-txt-sample.hpp"
#include "gles2-txt-target-2d.hpp"
#include "gles2-txt-target-cube.hpp"

#ifdef GX_DEBUG_GLES2
#define GX_DEBUG_GLES2_TARGET
#endif

void gearoenix::gles2::texture::Target::state_init() const noexcept
{
#ifdef GX_DEBUG_GLES2_TARGET
    gl::Loader::check_for_error();
#endif
    gl::Loader::clear_color(0.0f, 0.0f, 0.0f, 1.0f);

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

gearoenix::gles2::texture::Target::Target(engine::Engine* const e) noexcept
    : gl_e(e)
{
}

gearoenix::gles2::texture::Target::~Target() noexcept
{
    if (framebuffer_borrowed)
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

std::shared_ptr<gearoenix::render::texture::Target> gearoenix::gles2::texture::Target::construct(
    const core::Id id,
    engine::Engine* const e,
    const render::texture::Info& info,
    unsigned int w,
    unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    switch (info.t) {
    case render::texture::Type::Target2D:
        return Target2D::construct(id, e, info, w, h, call);
        //    case render::texture::Type::TargetCube:
        //        return TargetCube::construct(id, e, info, w, h, call);
    default:
        GXUNEXPECTED
    }
}

void gearoenix::gles2::texture::Target::bind() const noexcept
{
    if (-1 != depth_buffer)
        gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, depth_buffer);
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
    state_init();
}

void gearoenix::gles2::texture::Target::clear() const noexcept
{
    if (write_depth) {
        gl::Loader::depth_mask(GL_TRUE);
    } else {
        gl::Loader::depth_mask(GL_FALSE);
    }

    if (framebuffer_borrowed) {
        gl::Loader::clear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    } else {
        gl::Loader::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

void gearoenix::gles2::texture::Target::bind_texture(
    const render::texture::Target* const t,
    const gl::enumerated texture_unit) noexcept
{
    switch (t->get_target_type()) {
    case render::texture::Type::Target2D:
        reinterpret_cast<const Target2D*>(t)->bind_texture(texture_unit);
        break;
    default:
        GXUNEXPECTED
    }
}

void gearoenix::gles2::texture::Target::bind(const render::texture::Target* const target) noexcept
{
    switch (target->get_target_type()) {
    case render::texture::Type::Target2D:
        reinterpret_cast<const Target2D*>(target)->bind();
        break;
    default:
        GXUNEXPECTED
    }
}

void gearoenix::gles2::texture::Target::fetch_current_framebuffer() noexcept
{
    framebuffer_borrowed = true;
    gl::Loader::get_integerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
    gl::Loader::get_integerv(GL_RENDERBUFFER_BINDING, &depth_buffer);
}

void gearoenix::gles2::texture::Target::generate_framebuffer(
    const Texture2D* const txt,
    const unsigned int w,
    const unsigned int h) noexcept
{
    framebuffer_borrowed = false;
    gl::Loader::gen_renderbuffers(1, reinterpret_cast<gl::uint*>(&depth_buffer));
    gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, depth_buffer);
    gl::Loader::renderbuffer_storage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
    gl::Loader::gen_framebuffers(1, reinterpret_cast<gl::uint*>(&framebuffer));
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
    gl::Loader::framebuffer_renderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
    txt->bind();
    gl::Loader::framebuffer_texture_2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txt->get_texture_object(), 0);
    if (gl::Loader::check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        GXLOGF("Failed to create render target!")
    state_init();
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, 0);
}

#endif

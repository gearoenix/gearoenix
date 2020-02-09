#include "gles2-txt-target.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/gles2-eng-engine.hpp"
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
    // check of main render target
    if (texture_object == gl::uint(-1))
        return;
    gl_e->get_function_loader()->load([cf { framebuffer }, cr { depth_buffer }, ct { texture_object }] {
        if (cf != -1)
            gl::Loader::delete_framebuffers(1, reinterpret_cast<const gl::uint*>(&cf));
        if (cr != -1)
            gl::Loader::delete_renderbuffers(1, reinterpret_cast<const gl::uint*>(&cr));
        if (ct != gl::uint(-1))
            gl::Loader::delete_textures(1, &ct);
    });
    texture_object = gl::uint(-1);
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
    case render::texture::Type::TargetCube:
        return TargetCube::construct(id, e, info, w, h, call);
    default:
        GXUNEXPECTED
    }
}

void gearoenix::gles2::texture::Target::bind() const noexcept
{
    if (-1 != depth_buffer)
        gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, depth_buffer);
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
    //    gl::Loader::viewport(0, 0, static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));
    //    gl::Loader::scissor(0, 0, static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));
    state_init();

    // TODO viewport can be passed as an arg to this function
    GXUNIMPLEMENTED

    if (write_depth) {
        gl::Loader::depth_mask(GL_TRUE);
    } else {
        gl::Loader::depth_mask(GL_FALSE);
    }

    if (texture_object == gl::uint(-1)) {
        gl::Loader::clear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    } else {
        gl::Loader::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

void gearoenix::gles2::texture::Target::bind_texture(const gl::enumerated texture_unit) const noexcept
{
#ifdef GX_DEBUG_MODE
    if (texture_object == gl::uint(-1))
        GXLOGF("This is the main render target and can not bind as texture.")
#endif
    gl::Loader::active_texture(GL_TEXTURE0 + texture_unit);
    gl::Loader::bind_texture(GL_TEXTURE_2D, texture_object);
}

void gearoenix::gles2::texture::Target::bind_texture(
    const render::texture::Target* const t,
    const gl::enumerated texture_unit) noexcept
{
    switch (t->get_texture_type()) {
    case render::texture::Type::Target2D:
        reinterpret_cast<const Target2D*>(t)->bind_texture(texture_unit);
        break;
    default:
        GXUNEXPECTED
    }
}

void gearoenix::gles2::texture::Target::bind(const render::texture::Target* const target) noexcept
{
    switch (target->get_texture_type()) {
    case render::texture::Type::Target2D:
        reinterpret_cast<const Target2D*>(target)->bind();
    default:
        GXUNEXPECTED
    }
}

#endif

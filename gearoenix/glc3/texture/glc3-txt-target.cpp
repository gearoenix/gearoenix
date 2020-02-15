#include "glc3-txt-target.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-2d.hpp"
#include "glc3-txt-target-2d.hpp"
#include "glc3-txt-target-cube.hpp"

void gearoenix::glc3::texture::Target::state_init() const noexcept
{
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
}

gearoenix::glc3::texture::Target::Target(engine::Engine* const e) noexcept
    : gl_e(e)
{
}

gearoenix::glc3::texture::Target::~Target() noexcept
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

std::shared_ptr<gearoenix::render::texture::Target> gearoenix::glc3::texture::Target::construct(
    const core::Id id,
    engine::Engine* const e,
    const std::vector<render::texture::Info>& infos,
    unsigned int w,
    unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    switch (infos[0].t) {
    case render::texture::Type::Target2D:
        return Target2D::construct(id, e, infos, w, h, call);
        //    case render::texture::Type::TargetCube:
        //        return TargetCube::construct(id, e, infos, w, h, call);
    default:
        GXUNEXPECTED
    }
}

void gearoenix::glc3::texture::Target::bind() const noexcept
{
    if (-1 != depth_buffer)
        gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, depth_buffer);
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
    state_init();
}

void gearoenix::glc3::texture::Target::clear() const noexcept
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

void gearoenix::glc3::texture::Target::fetch_current_framebuffer() noexcept
{
    framebuffer_borrowed = true;
    gl::Loader::get_integerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
    gl::Loader::get_integerv(GL_RENDERBUFFER_BINDING, &depth_buffer);
}

void gearoenix::glc3::texture::Target::generate_framebuffer(
    const std::vector<render::texture::Info>& infos,
    const std::vector<std::shared_ptr<Texture2D>>& textures,
    const unsigned int, // this is needed for render buffer in future
    const unsigned int) noexcept
{
    framebuffer_borrowed = false;
    gl::Loader::gen_framebuffers(1, reinterpret_cast<gl::uint*>(&framebuffer));
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
    // TODO: check for need of depth buffer, It can be done by looping over infos for depth formats, if there was none so depth buffer must be created.
    for (std::size_t i = 0; i < textures.size(); ++i) {
        const auto& txt_info = infos[i];
        const auto& txt_fmt = txt_info.f;
        const auto& txt = textures[i];
        txt->bind();
        switch (txt_fmt) {
        case render::texture::TextureFormat::D32:
            gl::Loader::framebuffer_texture_2d(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, txt->get_texture_object(), 0);
            break;
        default:
            GXUNIMPLEMENTED
        }
    }
    if (gl::Loader::check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        GXLOGF("Failed to create render target!")
    state_init();
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, 0);
}

void gearoenix::glc3::texture::Target::bind(const render::texture::Target* const t) noexcept
{
    switch (t->get_target_type()) {
    case render::texture::Type::Target2D:
        reinterpret_cast<const Target2D*>(t)->bind();
        break;
    default:
        GXUNEXPECTED
    }
}

void gearoenix::glc3::texture::Target::bind_textures(
    const render::texture::Target* const t,
    const gl::enumerated* const texture_units,
    const std::size_t count) noexcept
{
    switch (t->get_target_type()) {
    case render::texture::Type::Target2D:
        reinterpret_cast<const Target2D*>(t)->bind_textures(texture_units, count);
        break;
    default:
        GXUNEXPECTED
    }
}

#endif

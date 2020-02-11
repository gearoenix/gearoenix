#include "glc3-txt-target.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/glc3-eng-engine.hpp"
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

gearoenix::glc3::texture::Target::~Target() noexcept
{
    if (texture_objects.empty()) // This is main render-target
        return;
    gl_e->get_function_loader()->load([cf { framebuffer }, cr { depth_buffer }, txts { move(texture_objects) }] {
        if (cf != -1)
            gl::Loader::delete_framebuffers(1, reinterpret_cast<const gl::uint*>(&cf));
        if (cr != -1)
            gl::Loader::delete_renderbuffers(1, reinterpret_cast<const gl::uint*>(&cr));
        for (auto ct : txts)
            gl::Loader::delete_textures(1, &ct);
    });
    texture_objects.clear();
    framebuffer = -1;
    depth_buffer = -1;
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

    if (texture_objects.empty()) {
        gl::Loader::clear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    } else {
        gl::Loader::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

void gearoenix::glc3::texture::Target::bind(const render::texture::Target* const t) noexcept
{
    switch (t->get_texture_type()) {
    case render::texture::Type::Target2D:
        static_cast<const Target2D*>(t)->bind();
        break;
    default:
        GXUNEXPECTED
    }
}

void gearoenix::glc3::texture::Target::bind_textures(
    const gl::enumerated* const texture_units, const std::size_t size) const noexcept
{
#ifdef GX_DEBUG_MODE
    if (size != texture_objects.size())
        GXLOGF("Different size for texture units is not acceptable.")
#endif
    for (std::size_t i = 0; i < size; ++i) {
        gl::Loader::active_texture(GL_TEXTURE0 + texture_units[i]);
        gl::Loader::bind_texture(GL_TEXTURE_2D, texture_objects[i]);
    }
}

void gearoenix::glc3::texture::Target::bind_textures(
    const render::texture::Target* const t,
    const gl::enumerated* const texture_units,
    const std::size_t count) noexcept
{
    switch (t->get_texture_type()) {
    case render::texture::Type::Target2D:
        static_cast<const Target2D*>(t)->bind_textures(texture_units, count);
        break;
    default:
        GXUNEXPECTED
    }
}

void gearoenix::glc3::texture::Target::fetch_current_framebuffer() noexcept
{
    gl::Loader::get_integerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
    gl::Loader::get_integerv(GL_RENDERBUFFER_BINDING, &depth_buffer);
}

void gearoenix::glc3::texture::Target::generate_framebuffer(
    const std::vector<render::texture::Info>& infos,
    const unsigned int w,
    const unsigned int h) noexcept
{
    gl::Loader::gen_framebuffers(1, reinterpret_cast<gl::uint*>(&framebuffer));
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
    // TODO: check for need of depth buffer, It can be done by looping over infos for depth formats, if there was none so depth buffer must be created.
    texture_objects.resize(infos.size());
    gl::Loader::gen_textures(static_cast<gearoenix::gl::sizei>(texture_objects.size()), texture_objects.data());
    for (std::size_t i = 0; i < texture_objects.size(); ++i) {
        const auto& txt_info = infos[i];
        const auto& txt_fmt = txt_info.f;
        const auto& txt = texture_objects[i];
        if (txt_fmt == render::texture::TextureFormat::D32) {
            gl::Loader::bind_texture(GL_TEXTURE_2D, txt);
            gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            /// The nearest filter in here is for workaround for some buggy vendors
            gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            gl::Loader::framebuffer_texture_2d(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, txt, 0);
        } else {
            GXUNIMPLEMENTED
        }
    }
    if (gl::Loader::check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        GXLOGF("Failed to create render target!")
    state_init();
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, 0);
}

#endif

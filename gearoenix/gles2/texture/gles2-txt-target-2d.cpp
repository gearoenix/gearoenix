#include "gles2-txt-target-2d.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-sample.hpp"

#ifdef GX_DEBUG_GLES2
#define GX_DEBUG_GLES2_TARGET
#endif

gearoenix::gles2::texture::Target2D::Target2D(const core::Id id, engine::Engine* const e) noexcept
    : render::texture::Texture(id, render::texture::Type::Target2D, e)
    , render::texture::Target2D(id, e)
    , gles2::texture::Target(e)
{
}

gearoenix::gles2::texture::Target2D::Target2D(engine::Engine* const e) noexcept
    : render::texture::Texture(core::asset::Manager::create_id(), render::texture::Type::Target2D, e)
    , render::texture::Target2D(asset_id, e)
    , gles2::texture::Target(e)
{
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    const auto* const sys_app = e->get_system_application();
    img_width = sys_app->get_window_width();
    img_height = sys_app->get_window_height();
    gl::Loader::get_integerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
    gl::Loader::get_integerv(GL_RENDERBUFFER_BINDING, &depth_buffer);
    state_init();
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
}

gearoenix::gles2::texture::Target2D::~Target2D() noexcept = default;

std::shared_ptr<gearoenix::gles2::texture::Target2D> gearoenix::gles2::texture::Target2D::construct(
    core::Id my_id,
    engine::Engine* e,
    const render::texture::Info& info,
    unsigned int w,
    unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<Target2D> result(new Target2D(my_id, e));
    result->img_width = w;
    result->img_height = h;
    if (info.f != render::texture::TextureFormat::D16)
        GXLOGF("GLES2 backend only supports 16bits depth attachment right now.")
    e->get_function_loader()->load([result, call, info] {
        gl::Loader::gen_framebuffers(1, reinterpret_cast<gl::uint*>(&(result->framebuffer)));
        gl::Loader::gen_renderbuffers(1, reinterpret_cast<gl::uint*>(&(result->depth_buffer)));
        gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, result->depth_buffer);
        gl::Loader::renderbuffer_storage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, result->img_width, result->img_height);
        gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, result->framebuffer);
        gl::Loader::framebuffer_renderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result->depth_buffer);
        gl::Loader::gen_textures(1, &(result->texture_object));
        switch (info.f) {
        case render::texture::TextureFormat::D16:
            gl::Loader::bind_texture(GL_TEXTURE_2D, result->texture_object);
            gl::Loader::tex_image_2d(GL_TEXTURE_2D, 0, GL_RGB, result->img_width, result->img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            gl::Loader::tex_parameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            gl::Loader::framebuffer_texture_2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result->texture_object, 0);
            break;
        default:
            GXUNEXPECTED
        }
        if (gl::Loader::check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            GXLOGF("Failed to create render target!")
        result->state_init();
        gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, 0);
    });
    return result;
}

#endif

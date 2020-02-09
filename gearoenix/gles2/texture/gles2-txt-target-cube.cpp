#include "gles2-txt-target-cube.hpp"
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

void gearoenix::gles2::texture::TargetCube::state_init() const noexcept
{
#ifdef GX_DEBUG_GLES2_TARGET
    gl::Loader::check_for_error();
#endif
    gl::Loader::clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    gl::Loader::enable(GL_CULL_FACE);
    gl::Loader::cull_face(GL_BACK);
    gl::Loader::enable(GL_BLEND);
    gl::Loader::blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl::Loader::enable(GL_DEPTH_TEST);
    gl::Loader::enable(GL_SCISSOR_TEST);
    gl::Loader::enable(GL_STENCIL_TEST);
//    gl::Loader::viewport(0, 0, static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));
//    gl::Loader::scissor(0, 0, static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));
#ifdef GX_DEBUG_GLES2_TARGET
    gl::Loader::check_for_error();
#endif
}

gearoenix::gles2::texture::TargetCube::TargetCube(const core::Id my_id, engine::Engine* const e) noexcept
    : render::texture::Texture(my_id, render::texture::Type::TargetCube, e)
    , render::texture::TargetCube(my_id, e)
{
}

gearoenix::gles2::texture::TargetCube::TargetCube(engine::Engine* const e) noexcept
    : render::texture::Texture(core::asset::Manager::create_id(), render::texture::Type::TargetCube, e)
    , render::texture::TargetCube(asset_id, e)
{
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    img_width = e->get_system_application()->get_window_width();
    img_height = e->get_system_application()->get_window_height();
    //    clipping_width = static_cast<core::Real>(img_width);
    //    clipping_height = static_cast<core::Real>(img_height);
    gl::Loader::get_integerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
    gl::Loader::get_integerv(GL_RENDERBUFFER_BINDING, &depth_buffer);
    state_init();
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
}

std::shared_ptr<gearoenix::gles2::texture::TargetCube> gearoenix::gles2::texture::TargetCube::construct(
    core::Id my_id,
    engine::Engine* e,
    const render::texture::Info& info,
    unsigned int w,
    unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept {
    GXUNIMPLEMENTED
}

gearoenix::gles2::texture::TargetCube::~TargetCube() noexcept
{
    if (texture_objects.size() == 0) // This is main render-target
        return;
    const auto cf = framebuffer;
    const auto cr = depth_buffer;
    render_engine->get_function_loader()->load([cf, cr, this] {
        if (cf != -1)
            gl::Loader::delete_framebuffers(1, reinterpret_cast<const gl::uint*>(&cf));
        if (cr != -1)
            gl::Loader::delete_renderbuffers(1, reinterpret_cast<const gl::uint*>(&cr));
        gl::Loader::delete_textures(static_cast<gearoenix::gl::sizei>(texture_objects.size()), texture_objects.data());
    });
}
#endif

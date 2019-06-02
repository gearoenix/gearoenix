#include "gles2-txt-target.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "../gles2.hpp"
#include "gles2-txt-sample.hpp"

#ifdef GX_DEBUG_GLES2
#define GX_DEBUG_GLES2_TARGET
#endif

gearoenix::gles2::texture::Target::Target(engine::Engine* const e) noexcept
    : render::texture::Target(core::asset::Manager::create_id(), e)
{
    img_width = e->get_system_application()->get_width();
    img_height = e->get_system_application()->get_height();
    gl::Loader::get_integerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
    gl::Loader::get_integerv(GL_RENDERBUFFER_BINDING, &depth_buffer);
    gl::Loader::clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    gl::Loader::enable(GL_CULL_FACE);
    gl::Loader::cull_face(GL_BACK);
    gl::Loader::enable(GL_BLEND);
    gl::Loader::blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl::Loader::enable(GL_DEPTH_TEST);
    gl::Loader::enable(GL_SCISSOR_TEST);
    gl::Loader::enable(GL_STENCIL_TEST);
    gl::Loader::viewport(0, 0, static_cast<gl::sizei>(img_width), static_cast<gl::sizei>(img_height));
    gl::Loader::scissor(0, 0, static_cast<gl::sizei>(img_width), static_cast<gl::sizei>(img_height));
#ifdef GX_DEBUG_GLES2_TARGET
    gl::Loader::check_for_error();
#endif
}

gearoenix::gles2::texture::Target::~Target() noexcept
{
    if (texture_object == 0) // This is main render-target
        return;
    GXUNIMPLEMENTED
}

void gearoenix::gles2::texture::Target::bind() const noexcept
{
    gl::Loader::bind_renderbuffer(GL_RENDERBUFFER, depth_buffer);
    gl::Loader::bind_framebuffer(GL_FRAMEBUFFER, framebuffer);
    gl::Loader::viewport(0, 0, static_cast<gl::sizei>(img_width), static_cast<gl::sizei>(img_height));
    gl::Loader::scissor(0, 0, static_cast<gl::sizei>(img_width), static_cast<gl::sizei>(img_height));
    gl::Loader::enable(GL_DEPTH_TEST);
    gl::Loader::depth_mask(GL_TRUE);
    gl::Loader::clear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

#endif

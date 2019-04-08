#include "gles2-txt-target.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-sample.hpp"

#ifdef GX_DEBUG_GLES2
#define GX_DEBUG_GLES2_TARGET
#endif

gearoenix::gles2::texture::Target::Target(const std::shared_ptr<engine::Engine>& e)
	: render::texture::Target(e->get_system_application()->get_asset_manager()->create_id(), e)
{
	const std::shared_ptr<system::Application> &sysapp = e->get_system_application();
	const float win_width = static_cast<float>(sysapp->get_width());
	const float win_height = static_cast<float>(sysapp->get_height());
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
	gl::Loader::viewport(0, 0, static_cast<gl::sizei>(win_width), static_cast<gl::sizei>(win_height));
	gl::Loader::scissor(0, 0, static_cast<gl::sizei>(win_width), static_cast<gl::sizei>(win_height));
#ifdef GX_DEBUG_GLES2
	gl::Loader::check_for_error();
#endif
}

// todo: deconstructor must consider texture-object if it was zero then it is main render-target

#endif
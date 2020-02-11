#include "gles2-txt-target-2d.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
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
    , base(new gles2::texture::Target(e))
{
}

gearoenix::gles2::texture::Target2D::Target2D(const Target2D& o) noexcept
    : render::texture::Texture(o.asset_id, render::texture::Type::Target2D, o.render_engine)
    , render::texture::Target2D(o.asset_id, o.render_engine)
    , base(o.base)
{
    clipping_width = o.clipping_width;
    clipping_height = o.clipping_height;
    clipping_starting_x = o.clipping_starting_x;
    clipping_starting_y = o.clipping_starting_y;
    img_width = o.img_width;
    img_height = o.img_height;
}

gearoenix::gles2::texture::Target2D::Target2D(engine::Engine* const e) noexcept
    : render::texture::Texture(core::asset::Manager::create_id(), render::texture::Type::Target2D, e)
    , render::texture::Target2D(asset_id, e)
    , base(new gles2::texture::Target(e))
{
#ifdef GX_DEBUG_GLES2
    gl::Loader::check_for_error();
#endif
    const auto* const sys_app = e->get_system_application();
    img_width = sys_app->get_window_width();
    img_height = sys_app->get_window_height();
    clipping_width = static_cast<core::Real>(img_width);
    clipping_height = static_cast<core::Real>(img_height);
    base->fetch_current_framebuffer();
    base->state_init();
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
    e->get_function_loader()->load([result, call, w, h, info] {
        result->base->generate_framebuffer(info, w, h);
    });
    return result;
}

void gearoenix::gles2::texture::Target2D::bind() const noexcept
{
    base->bind();
    gl::Loader::viewport(
        static_cast<gl::sizei>(clipping_starting_x), static_cast<gl::sizei>(clipping_starting_y),
        static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));
    gl::Loader::scissor(
        static_cast<gl::sizei>(clipping_starting_x), static_cast<gl::sizei>(clipping_starting_y),
        static_cast<gl::sizei>(clipping_width), static_cast<gl::sizei>(clipping_height));
    base->clear();
}

void gearoenix::gles2::texture::Target2D::bind_texture(const gl::enumerated texture_unit) const noexcept
{
    base->bind_texture(texture_unit);
}

gearoenix::render::texture::Target* gearoenix::gles2::texture::Target2D::clone() const noexcept
{
    return new Target2D(*this);
}

#endif

#include "gles2-txt-target-2d.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-txt-2d.hpp"
#include "gles2-txt-sample.hpp"

#ifdef GX_DEBUG_GLES2
#define GX_DEBUG_GLES2_TARGET
#endif

gearoenix::gles2::texture::Target2D::Target2D(
    const core::Id id,
    engine::Engine* const e,
    const render::texture::Info& info,
    const unsigned int w,
    const unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : render::texture::Target2D(id, e)
    , base(new gles2::texture::Target(e))
    , txt(Texture2D::construct(core::asset::Manager::create_id(), e, info, w, h, call))
{
}

gearoenix::gles2::texture::Target2D::Target2D(const Target2D& o) noexcept
    : render::texture::Target2D(o.target_id, o.e)
    , base(o.base)
    , txt(o.txt)
{
    clipping_width = o.clipping_width;
    clipping_height = o.clipping_height;
    clipping_starting_x = o.clipping_starting_x;
    clipping_starting_y = o.clipping_starting_y;
}

gearoenix::gles2::texture::Target2D::Target2D(engine::Engine* const e) noexcept
    : render::texture::Target2D(0, e)
    , base(new gles2::texture::Target(e))
    , txt(nullptr)
{
    const auto* const sys_app = e->get_system_application();
    clipping_width = static_cast<core::Real>(sys_app->get_window_width());
    clipping_height = static_cast<core::Real>(sys_app->get_window_height());
    base->fetch_current_framebuffer();
    base->state_init();
}

gearoenix::gles2::texture::Target2D::~Target2D() noexcept = default;

std::shared_ptr<gearoenix::gles2::texture::Target2D> gearoenix::gles2::texture::Target2D::construct(
    const core::Id id,
    engine::Engine* const e,
    const render::texture::Info& info,
    const unsigned int w,
    const unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<Target2D> result(new Target2D(id, e, info, w, h, call));
    if (info.f != render::texture::TextureFormat::D16)
        GXLOGF("GLES2 backend only supports 16bits depth attachment right now.")
    e->get_function_loader()->load([result, call, w, h] {
        result->base->generate_framebuffer(result->txt.get(), w, h);
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
#ifdef GX_DEBUG_MODE
    if (txt == nullptr)
        GXLOGF("This is the main render target and can not bind as texture.")
#endif
    txt->bind(texture_unit);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::gles2::texture::Target2D::get_texture_2d(const std::size_t index) const noexcept
{
#ifdef GX_DEBUG_GLES2_TARGET
    if (index != 0 && txt == nullptr)
        GXUNEXPECTED
#endif
    return txt;
}

std::size_t gearoenix::gles2::texture::Target2D::get_textures_count() const noexcept
{
    return txt == nullptr ? 0 : 1;
}

gearoenix::render::texture::Target* gearoenix::gles2::texture::Target2D::clone() const noexcept
{
    return new Target2D(*this);
}

#endif

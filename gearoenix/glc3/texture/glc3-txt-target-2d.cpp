#include "glc3-txt-target-2d.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-sample.hpp"
#include "glc3-txt-target.hpp"

gearoenix::glc3::texture::Target2D::Target2D(const core::Id id, engine::Engine* const e) noexcept
    : render::texture::Texture(id, render::texture::Type::Target2D, e)
    , render::texture::Target2D(id, e)
    , base(new glc3::texture::Target(e))
{
}

gearoenix::glc3::texture::Target2D::Target2D(const Target2D& o) noexcept
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

gearoenix::glc3::texture::Target2D::Target2D(engine::Engine* const e) noexcept
    : render::texture::Texture(core::asset::Manager::create_id(), render::texture::Type::Target2D, e)
    , render::texture::Target2D(asset_id, e)
    , base(new glc3::texture::Target(e))
{
    const auto* const sys_app = e->get_system_application();
    img_width = sys_app->get_window_width();
    img_height = sys_app->get_window_height();
    clipping_width = static_cast<core::Real>(img_width);
    clipping_height = static_cast<core::Real>(img_height);
    base->fetch_current_framebuffer();
    base->state_init();
}

gearoenix::glc3::texture::Target2D::~Target2D() noexcept = default;

std::shared_ptr<gearoenix::glc3::texture::Target2D> gearoenix::glc3::texture::Target2D::construct(
    core::Id my_id,
    engine::Engine* e,
    const std::vector<render::texture::Info>& infos,
    unsigned int w,
    unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    const std::shared_ptr<Target2D> result(new Target2D(my_id, e));
    result->img_width = w;
    result->img_height = h;
    e->get_function_loader()->load([result, infos, w, h, call] {
        result->base->generate_framebuffer(infos, w, h);
    });
    return result;
}

void gearoenix::glc3::texture::Target2D::bind() const noexcept
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

void gearoenix::glc3::texture::Target2D::bind_textures(
    const gl::enumerated* const texture_units,
    const std::size_t count) const noexcept
{
    base->bind_textures(texture_units, count);
}

gearoenix::render::texture::Target* gearoenix::glc3::texture::Target2D::clone() const noexcept
{
    return new Target2D(*this);
}

#endif

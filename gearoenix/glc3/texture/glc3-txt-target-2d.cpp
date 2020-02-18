#include "glc3-txt-target-2d.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-txt-2d.hpp"
#include "glc3-txt-sample.hpp"
#include "glc3-txt-target.hpp"

gearoenix::glc3::texture::Target2D::Target2D(
    const core::Id id,
    engine::Engine* const e,
    const std::vector<render::texture::Info>& infos,
    const unsigned int w,
    const unsigned int h,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : render::texture::Target2D(id, e)
    , base(new glc3::texture::Target(e))
    , textures(create_textures(e, infos, w, h, call))
{
    clipping_width = static_cast<core::Real>(w);
    clipping_height = static_cast<core::Real>(h);
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
#ifdef GX_DEBUG_MODE
    if (count != textures.size())
        GXUNEXPECTED
#endif
    for (std::size_t i = 0; i < count; ++i)
        textures[i]->bind(texture_units[i]);
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::glc3::texture::Target2D::get_texture_2d(const std::size_t index) const noexcept
{
    return textures[index];
}

std::size_t gearoenix::glc3::texture::Target2D::get_textures_count() const noexcept
{
    return textures.size();
}

gearoenix::render::texture::Target* gearoenix::glc3::texture::Target2D::clone() const noexcept
{
    return new Target2D(*this);
}

#endif

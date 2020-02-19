#include "glc3-txt-texture.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "glc3-txt-2d.hpp"
#include "glc3-txt-target.hpp"

void gearoenix::glc3::texture::Texture::bind(
    const render::texture::Texture* const t,
    const gl::enumerated texture_unit) noexcept
{
    switch (t->get_texture_type()) {
    case render::texture::Type::Texture2D:
        reinterpret_cast<const texture::Texture2D*>(t)->bind(texture_unit);
        return;
    default:
        GXUNEXPECTED
    }
}

#endif
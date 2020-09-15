#include "gx-gles2-txt-texture.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "gx-gles2-txt-2d.hpp"
#include "gx-gles2-txt-cube.hpp"

void gearoenix::gles2::texture::Texture::bind(
    const render::texture::Texture* const t,
    const gl::enumerated texture_unit) noexcept
{
    switch (t->get_texture_type()) {
    case render::texture::Type::Texture2D:
        static_cast<const texture::Texture2D*>(t)->bind(texture_unit);
        return;
    case render::texture::Type::TextureCube:
        static_cast<const texture::TextureCube*>(t)->bind(texture_unit);
        return;
    default:
        GX_UNEXPECTED
    }
}

#endif

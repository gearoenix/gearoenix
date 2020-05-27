#include "glc3-txt-texture.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "glc3-txt-2d.hpp"
#include "glc3-txt-cube.hpp"

void gearoenix::glc3::texture::Texture::bind(
    const render::texture::Texture* const t,
    const gl::enumerated texture_unit) noexcept
{
    switch (t->get_texture_type()) {
    case render::texture::Type::Texture2D:
        static_cast<const texture::Texture2D*>(t)->bind(texture_unit);
        return;
    default:
        GX_UNEXPECTED
    }
}

void gearoenix::glc3::texture::Texture::generate_mipmap(const render::texture::Texture* t) noexcept
{
    switch (t->get_texture_type()) {
    case render::texture::Type::Texture2D:
        static_cast<const texture::Texture2D*>(t)->generate_mipmap();
        return;
    case render::texture::Type::TextureCube:
        static_cast<const texture::TextureCube*>(t)->generate_mipmap();
        return;
    default:
        GX_UNEXPECTED
    }
}

#endif
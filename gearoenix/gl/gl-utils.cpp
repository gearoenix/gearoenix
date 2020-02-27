#include "gl-utils.hpp"
#ifdef GX_USE_OPENGL
#include "../system/sys-log.hpp"
#include "gl-constants.hpp"

gearoenix::gl::enumerated gearoenix::gl::convert(const render::texture::Face f) noexcept
{
    switch (f) {
    case render::texture::Face::PositiveX:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    case render::texture::Face::NegativeX:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    case render::texture::Face::PositiveY:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    case render::texture::Face::NegativeY:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    case render::texture::Face::PositiveZ:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    case render::texture::Face::NegativeZ:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    }
    GXUNEXPECTED
}

#endif

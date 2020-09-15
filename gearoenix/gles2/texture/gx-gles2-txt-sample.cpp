#include "gx-gles2-txt-sample.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/gx-sys-log.hpp"

gearoenix::gles2::texture::SampleInfo::SampleInfo(const render::texture::SampleInfo& o) noexcept
{
    switch (o.mag_filter) {
    case render::texture::Filter::Nearest:
        mag_filter = GL_NEAREST;
        break;
    case render::texture::Filter::Linear:
        mag_filter = GL_LINEAR;
        break;
    default:
        GX_UNEXPECTED
    }
    switch (o.min_filter) {
    case render::texture::Filter::Nearest:
        min_filter = GL_NEAREST;
        break;
    case render::texture::Filter::Linear:
        min_filter = GL_LINEAR;
        break;
    case render::texture::Filter::LinearMipmapLinear:
        min_filter = GL_LINEAR_MIPMAP_LINEAR;
        break;
    default:
        GX_UNEXPECTED
    }
    const auto fun = [](render::texture::Wrap w) {
        switch (w) {
        case render::texture::Wrap::ClampToEdge:
            return GL_CLAMP_TO_EDGE;
        case render::texture::Wrap::Mirror:
            return GL_MIRRORED_REPEAT;
        case render::texture::Wrap::Repeat:
            return GL_REPEAT;
        default:
            GX_UNEXPECTED
        }
    };
    wrap_s = fun(o.wrap_s);
    wrap_t = fun(o.wrap_t);
    wrap_r = fun(o.wrap_r);
}
#endif
#include "gx-glc3-txt-sample.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../system/gx-sys-log.hpp"

gearoenix::glc3::texture::SampleInfo::SampleInfo(const render::texture::SampleInfo& o) noexcept
{
    const auto min_fil_fun = [](render::texture::Filter f) {
        switch (f) {
        case render::texture::Filter::Nearest:
            return GL_NEAREST;
        case render::texture::Filter::NearestMipmapLinear:
            return GL_NEAREST_MIPMAP_LINEAR;
        case render::texture::Filter::NearestMipmapNearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case render::texture::Filter::Linear:
            return GL_LINEAR;
        case render::texture::Filter::LinearMipmapLinear:
            return GL_LINEAR_MIPMAP_LINEAR;
        case render::texture::Filter::LinearMipmapNearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        default:
            GX_UNEXPECTED
        }
    };

    const auto mag_fil_fun = [](render::texture::Filter f) {
        switch (f) {
        case render::texture::Filter::Nearest:
            return GL_NEAREST;
        case render::texture::Filter::Linear:
            return GL_LINEAR;
        default:
            GX_UNEXPECTED
        }
    };

#define GX_HELPER(name) name##_filter = static_cast<float>(name##_fil_fun(o.name##_filter));

    GX_HELPER(mag)
    GX_HELPER(min)

#undef GX_HELPER

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

#define GX_HELPER(name) wrap_##name = fun(o.wrap_##name);

    GX_HELPER(s);
    GX_HELPER(t);
    GX_HELPER(r);

#undef GX_HELPER
}

#endif
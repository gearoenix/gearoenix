#include "glc3-txt-sample.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../system/sys-log.hpp"

gearoenix::glc3::texture::SampleInfo::SampleInfo(const render::texture::SampleInfo& o) noexcept
{
    const auto min_filfun = [](render::texture::Filter::Id f) {
        switch (f) {
        case render::texture::Filter::NEAREST:
            return GL_NEAREST;
        case render::texture::Filter::NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case render::texture::Filter::NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case render::texture::Filter::LINEAR:
            return GL_LINEAR;
        case render::texture::Filter::LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
        case render::texture::Filter::LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        default:
            GXUNEXPECTED
        }
    };

    const auto mag_filfun = [](render::texture::Filter::Id f) {
        switch (f) {
        case render::texture::Filter::NEAREST:
            return GL_NEAREST;
        case render::texture::Filter::LINEAR:
            return GL_LINEAR;
        default:
            GXUNEXPECTED
        }
    };

#define GX_HELPER(name) name##_filter = static_cast<float>(name##_filfun(o.name##_filter));

    GX_HELPER(mag)
    GX_HELPER(min)

#undef GX_HELPER

    const auto fun = [](render::texture::Wrap::Id w) {
        switch (w) {
        case render::texture::Wrap::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case render::texture::Wrap::MIRROR:
            return GL_MIRRORED_REPEAT;
        case render::texture::Wrap::REPEAT:
            return GL_REPEAT;
        default:
            GXUNEXPECTED
        }
    };

#define GX_HELPER(name) wrap_##name = fun(o.wrap_##name);

    GX_HELPER(s);
    GX_HELPER(t);
    GX_HELPER(r);

#undef GX_HELPER
}
#endif
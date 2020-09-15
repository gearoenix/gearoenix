#ifndef GEAROENIX_GLC3_TEXTURE_SAMPLE_HPP
#define GEAROENIX_GLC3_TEXTURE_SAMPLE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gx-gl-constants.hpp"
#include "../../gl/gx-gl-types.hpp"
#include "../../render/texture/gx-rnd-txt-sample.hpp"

namespace gearoenix::glc3::texture {
struct SampleInfo {
    float min_filter = GL_LINEAR;
    float mag_filter = GL_LINEAR;
    gl::sint wrap_s = GL_REPEAT;
    gl::sint wrap_t = GL_REPEAT;
    gl::sint wrap_r = GL_REPEAT;

    SampleInfo() noexcept = default;
    explicit SampleInfo(const render::texture::SampleInfo& o) noexcept;
};
}
#endif
#endif

#ifndef GEAROENIX_GLES2_TEXTURE_SAMPLE_HPP
#define GEAROENIX_GLES2_TEXTURE_SAMPLE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gx-gl-constants.hpp"
#include "../../gl/gx-gl-types.hpp"
#include "../../render/texture/gx-rnd-txt-sampler.hpp"

namespace gearoenix::gles2::texture {
struct SamplerInfo {
    float min_filter = GL_LINEAR_MIPMAP_LINEAR;
    float mag_filter = GL_LINEAR;
    gl::sint wrap_s = GL_REPEAT;
    gl::sint wrap_t = GL_REPEAT;
    gl::sint wrap_r = GL_REPEAT;

    SamplerInfo() noexcept = default;
    explicit SamplerInfo(const render::texture::SamplerInfo& o) noexcept;
};
}
#endif
#endif

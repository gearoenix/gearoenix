#ifndef GEAROENIX_GLES2_TEXTURE_SAMPLE_HPP
#define GEAROENIX_GLES2_TEXTURE_SAMPLE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-sample.hpp"

namespace gearoenix::gles2::texture {
struct SampleInfo {
    float min_filter = GL_LINEAR_MIPMAP_LINEAR;
    float mag_filter = GL_LINEAR;
    gl::sint wrap_s = GL_REPEAT;
    gl::sint wrap_t = GL_REPEAT;
    gl::sint wrap_r = GL_REPEAT;

    SampleInfo() noexcept = default;
    explicit SampleInfo(const render::texture::SampleInfo& o) noexcept;

    [[nodiscard]] bool needs_mipmap() const noexcept;
};
}
#endif
#endif

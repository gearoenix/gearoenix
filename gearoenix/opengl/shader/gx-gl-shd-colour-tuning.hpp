#pragma once
#include "gx-gl-shader.hpp"
#if GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct ColourTuning final : Shader {
    GX_GL_UNIFORM_TEXTURE(source_texture);
    GX_GL_UNIFORM_TEXTURE(colour_lut);
    GX_GL_UNIFORM_TEXTURE(blue_noise);
    GX_GL_UNIFORM_FLOAT(frame_index, 1);
    GX_GL_UNIFORM_FLOAT(lut_size, 1);
    GX_GL_UNIFORM_INT(tonemap_mode, 1);
    GX_GL_UNIFORM_FLOAT(agx_peak_ratio, 1);
    GX_GL_UNIFORM_VECTOR(noise_uv_scale, 2, 1);

public:
    ColourTuning();
    ~ColourTuning() override;
    void bind(uint& current_shader) const override;
};
}

#endif

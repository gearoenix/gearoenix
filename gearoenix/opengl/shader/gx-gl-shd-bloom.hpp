#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <array>
#include <optional>

namespace gearoenix::gl::shader {
struct BloomPrefilter final : Shader {
    GX_GL_UNIFORM_VECTOR(texel_size, 2, 1);
    GX_GL_UNIFORM_VECTOR(scatter_clamp_max_threshold_threshold_knee, 4, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);

public:
    BloomPrefilter();
    ~BloomPrefilter() override;
    void bind(uint& current_shader) const override;
};

struct BloomHorizontal final : Shader {
    GX_GL_UNIFORM_VECTOR(texel_size_mip_index, 3, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);

public:
    BloomHorizontal();
    ~BloomHorizontal() override;
    void bind(uint& current_shader) const override;
};

struct BloomVertical final : Shader {
    GX_GL_UNIFORM_VECTOR(texel_size_mip_index, 3, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);

public:
    BloomVertical();
    ~BloomVertical() override;
    void bind(uint& current_shader) const override;
};

struct BloomUpsampler final : Shader {
    GX_GL_UNIFORM_VECTOR(scatter_src_mip_index, 2, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);
    GX_GL_UNIFORM_TEXTURE(low_texture);

public:
    BloomUpsampler();
    ~BloomUpsampler() override;
    void bind(uint& current_shader) const override;
};
}
#endif
#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <array>
#include <optional>

namespace gearoenix::gl::shader {
struct BloomPrefilter final : public Shader {
    GX_GL_UNIFORM_VECTOR(texel_size, 2, 1);
    GX_GL_UNIFORM_VECTOR(scatter_clamp_max_threshold_threshold_knee, 4, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);

public:
    explicit BloomPrefilter(Engine& e);
    ~BloomPrefilter() final;
    void bind(uint& current_shader) const final;
};

struct BloomHorizontal final : public Shader {
    GX_GL_UNIFORM_VECTOR(texel_size_mip_index, 3, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);

public:
    explicit BloomHorizontal(Engine& e);
    ~BloomHorizontal() final;
    void bind(uint& current_shader) const final;
};

struct BloomVertical final : public Shader {
    GX_GL_UNIFORM_VECTOR(texel_size_mip_index, 3, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);

public:
    explicit BloomVertical(Engine& e);
    ~BloomVertical() final;
    void bind(uint& current_shader) const final;
};

struct BloomUpsampler final : public Shader {
    GX_GL_UNIFORM_VECTOR(scatter_src_mip_index, 2, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);
    GX_GL_UNIFORM_TEXTURE(low_texture);

public:
    explicit BloomUpsampler(Engine& e);
    ~BloomUpsampler() final;
    void bind(uint& current_shader) const final;
};
}
#endif
#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct DeferredPbrTransparent final : Shader {
    GX_GL_UNIFORM_VECTOR(screen_uv_move_reserved, 4, 1);
    GX_GL_UNIFORM_TEXTURE(albedo_metallic);
    GX_GL_UNIFORM_TEXTURE(position_depth);
    GX_GL_UNIFORM_TEXTURE(normal_ao);
    GX_GL_UNIFORM_TEXTURE(emission_roughness);
    GX_GL_UNIFORM_TEXTURE(ssao_resolved);

public:
    explicit DeferredPbrTransparent(Engine& e);
    ~DeferredPbrTransparent() override;
    void bind(uint& current_shader) const override;
};
}

#endif
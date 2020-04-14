#ifndef GEAROENIX_GLES2_SHADER_SKYBOX_CUBE_HPP
#define GEAROENIX_GLES2_SHADER_SKYBOX_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../gles2.hpp"
#include "gles2-shd-shader.hpp"
#include <string>

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::shader {
class SkyboxCube final : public Shader {
    GX_GLES2_UNIFORM_FLOAT(material_alpha, 1)
    GX_GLES2_UNIFORM_FLOAT(material_alpha_cutoff, 1)
    GX_GLES2_UNIFORM_TEXTURE(material_color)
    GX_GLES2_UNIFORM_FLOAT(camera_hdr_tune_mapping, 1)
    GX_GLES2_UNIFORM_FLOAT(camera_gamma_correction, 1)
    GX_GLES2_UNIFORM_MATRIX(effect_mvp, 4, 1)
public:
    SkyboxCube(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~SkyboxCube() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif

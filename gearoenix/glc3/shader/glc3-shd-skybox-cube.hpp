#ifndef GEAROENIX_GLC3_SHADER_SKYBOX_CUBE_HPP
#define GEAROENIX_GLC3_SHADER_SKYBOX_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../glc3.hpp"
#include "glc3-shd-shader.hpp"
#include <string>

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class SkyboxCube final : public Shader {
    GX_GLC3_UNIFORM_FLOAT(material_alpha, 1)
    GX_GLC3_UNIFORM_FLOAT(material_alpha_cutoff, 1)
    GX_GLC3_UNIFORM_TEXTURE(material_color)
    GX_GLC3_UNIFORM_FLOAT(camera_hdr_tune_mapping, 1)
    GX_GLC3_UNIFORM_FLOAT(camera_gamma_correction, 1)
    GX_GLC3_UNIFORM_MATRIX(effect_mvp, 4, 1)

public:
    SkyboxCube(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~SkyboxCube() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif

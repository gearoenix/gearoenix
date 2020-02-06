#ifndef GEAROENIX_GLC3_SHADER_SKYBOX_EQUIRECTANGULAR_HPP
#define GEAROENIX_GLC3_SHADER_SKYBOX_EQUIRECTANGULAR_HPP
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
class SkyboxEquirectangular : public Shader {
    GX_GLC3_UNIFORM_FLOAT(material_alpha, 1)
    GX_GLC3_UNIFORM_FLOAT(material_alpha_cutoff, 1)
    GX_GLC3_UNIFORM_TEXTURE(material_color)
    GX_GLC3_UNIFORM_MATRIX(effect_mvp, 4, 1)

public:
    SkyboxEquirectangular(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~SkyboxEquirectangular() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif

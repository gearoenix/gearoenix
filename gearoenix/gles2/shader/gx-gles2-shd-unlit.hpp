#ifndef GEAROENIX_GLES2_SHADER_UNLIT_HPP
#define GEAROENIX_GLES2_SHADER_UNLIT_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/gx-cr-static.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../gx-gles2.hpp"
#include "gx-gles2-shd-shader.hpp"
#include <string>

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::shader {
class Unlit final : public Shader {
    GX_GLES2_UNIFORM_FLOAT(material_alpha, 1)
    GX_GLES2_UNIFORM_FLOAT(material_alpha_cutoff, 1)
    GX_GLES2_UNIFORM_TEXTURE(material_color)
    GX_GLES2_UNIFORM_MATRIX(effect_mvp, 4, 1)
public:
    Unlit(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Unlit() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif

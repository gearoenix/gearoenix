#ifndef GEAROENIX_GLES2_SHADER_IRRADIANCE_CONVOLUTER_HPP
#define GEAROENIX_GLES2_SHADER_IRRADIANCE_CONVOLUTER_HPP
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
class IrradianceConvoluter final : public Shader {
    GX_GLES2_UNIFORM_TEXTURE(environment)
public:
    IrradianceConvoluter(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~IrradianceConvoluter() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif

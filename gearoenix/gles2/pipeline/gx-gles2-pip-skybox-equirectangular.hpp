#ifndef GEAROENIX_GLES2_PIPELINE_SKYBOX_EQUIRECTANGULAR_HPP
#define GEAROENIX_GLES2_PIPELINE_SKYBOX_EQUIRECTANGULAR_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/pipeline/gx-rnd-pip-skybox-equirectangular.hpp"

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::shader {
class SkyboxEquirectangular;
}

namespace gearoenix::gles2::pipeline {
class SkyboxEquirectangular final : public render::pipeline::SkyboxEquirectangular {
private:
    const std::shared_ptr<shader::SkyboxEquirectangular> shd;
    std::weak_ptr<SkyboxEquirectangular> self;
    SkyboxEquirectangular(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<SkyboxEquirectangular> construct(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~SkyboxEquirectangular() noexcept final;
    [[nodiscard]] render::pipeline::ResourceSet* create_resource_set() const noexcept final;
};
}

#endif
#endif

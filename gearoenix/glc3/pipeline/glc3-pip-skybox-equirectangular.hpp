#ifndef GEAROENIX_GLC3_PIPELINE_SKYBOX_EQUIRECTANGULAR_HPP
#define GEAROENIX_GLC3_PIPELINE_SKYBOX_EQUIRECTANGULAR_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/rnd-pip-skybox-equirectangular.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class SkyboxEquirectangular;
}

namespace gearoenix::glc3::pipeline {
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

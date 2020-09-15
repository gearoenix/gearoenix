#ifndef GEAROENIX_GLC3_PIPELINE_SKYBOX_CUBE_HPP
#define GEAROENIX_GLC3_PIPELINE_SKYBOX_CUBE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/gx-rnd-pip-skybox-cube.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class SkyboxCube;
}

namespace gearoenix::glc3::pipeline {
class SkyboxCube final : public render::pipeline::SkyboxCube {
private:
    const std::shared_ptr<shader::SkyboxCube> shd;
    std::weak_ptr<SkyboxCube> self;

    SkyboxCube(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<SkyboxCube> construct(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~SkyboxCube() noexcept final;
    [[nodiscard]] render::pipeline::ResourceSet* create_resource_set() const noexcept final;
};
}

#endif
#endif

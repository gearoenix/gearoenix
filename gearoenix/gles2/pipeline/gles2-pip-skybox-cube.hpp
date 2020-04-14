#ifndef GEAROENIX_GLES2_PIPELINE_SKYBOX_CUBE_HPP
#define GEAROENIX_GLES2_PIPELINE_SKYBOX_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/pipeline/rnd-pip-skybox-cube.hpp"

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::shader {
class SkyboxCube;
}

namespace gearoenix::gles2::pipeline {
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

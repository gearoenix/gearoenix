#ifndef GEAROENIX_GLES2_PIPELINE_SKYBOX_CUBE_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_SKYBOX_CUBE_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/pipeline/rnd-pip-skybox-cube-resource-set.hpp"

namespace gearoenix::gles2::shader {
class SkyboxCube;
}

namespace gearoenix::gles2::pipeline {
class SkyboxCube;
class ResourceSet;
class SkyboxCubeResourceSet final : public render::pipeline::SkyboxCubeResourceSet {
    GX_GET_UCPTR_PRV(gles2::pipeline::ResourceSet, base)
public:
    SkyboxCubeResourceSet(const std::shared_ptr<shader::SkyboxCube>& shd, std::shared_ptr<SkyboxCube const> pip) noexcept;
    ~SkyboxCubeResourceSet() noexcept final;
    void bind_final(gl::uint& bound_shader_program) const noexcept;
};
}

#endif
#endif

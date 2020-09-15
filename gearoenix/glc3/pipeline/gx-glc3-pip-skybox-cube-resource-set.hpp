#ifndef GEAROENIX_GLC3_PIPELINE_SKYBOX_CUBE_RESOURCE_SET_HPP
#define GEAROENIX_GLC3_PIPELINE_SKYBOX_CUBE_RESOURCE_SET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../gl/gx-gl-types.hpp"
#include "../../render/pipeline/gx-rnd-pip-skybox-cube-resource-set.hpp"

namespace gearoenix::glc3::shader {
class SkyboxCube;
}

namespace gearoenix::glc3::pipeline {
class SkyboxCube;
class ResourceSet;
class SkyboxCubeResourceSet final : public render::pipeline::SkyboxCubeResourceSet {
    GX_GET_UCPTR_PRV(glc3::pipeline::ResourceSet, base)
public:
    SkyboxCubeResourceSet(const std::shared_ptr<shader::SkyboxCube>& shd, std::shared_ptr<SkyboxCube const> pip) noexcept;
    ~SkyboxCubeResourceSet() noexcept final;
    void bind_final(gl::uint& bound_shader_program) const noexcept;
};
}

#endif
#endif

#ifndef GEAROENIX_GLES2_PIPELINE_SKYBOX_EQUIRECTANGULAR_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_SKYBOX_EQUIRECTANGULAR_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../render/pipeline/rnd-pip-skybox-equirectangular-resource-set.hpp"
#include "gles2-pip-resource-set.hpp"

namespace gearoenix::gles2::shader {
class SkyboxEquirectangular;
}

namespace gearoenix::gles2::pipeline {
class SkyboxEquirectangularResourceSet : public ResourceSet, public render::pipeline::SkyboxEquirectangularResourceSet {
public:
    explicit SkyboxEquirectangularResourceSet(const std::shared_ptr<shader::SkyboxEquirectangular>& shd) noexcept;
    void bind(gl::uint& bound_shader_program) const noexcept final;
};
}

#endif
#endif

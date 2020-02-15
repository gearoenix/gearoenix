#ifndef GEAROENIX_GLES2_PIPELINE_SKYBOX_EQUIRECTANGULAR_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_SKYBOX_EQUIRECTANGULAR_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/pipeline/rnd-pip-skybox-equirectangular-resource-set.hpp"

namespace gearoenix::gles2::shader {
class SkyboxEquirectangular;
}

namespace gearoenix::gles2::pipeline {
class SkyboxEquirectangular;
class ResourceSet;
class SkyboxEquirectangularResourceSet : public render::pipeline::SkyboxEquirectangularResourceSet {
    GX_GET_UCPTR_PRV(gles2::pipeline::ResourceSet, base)
public:
    SkyboxEquirectangularResourceSet(const std::shared_ptr<shader::SkyboxEquirectangular>& shd, std::shared_ptr<SkyboxEquirectangular const> pip) noexcept;
    ~SkyboxEquirectangularResourceSet() noexcept final;
    void bind_final(gl::uint& bound_shader_program) const noexcept;
};
}

#endif
#endif

#include "gles2-pip-skybox-equirectangular.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gles2-eng-engine.hpp"
#include "../shader/gles2-shd-skybox-equirectangular.hpp"
#include "gles2-pip-skybox-equirectangular-resource-set.hpp"

gearoenix::gles2::pipeline::SkyboxEquirectangular::SkyboxEquirectangular(
    engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::SkyboxEquirectangular(e, c)
    , shd(new shader::SkyboxEquirectangular(e, c))
{
}

gearoenix::gles2::pipeline::SkyboxEquirectangular::~SkyboxEquirectangular() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::gles2::pipeline::SkyboxEquirectangular::create_resource_set() const noexcept
{
    return new SkyboxEquirectangularResourceSet(shd, shared_from_this());
}

#endif
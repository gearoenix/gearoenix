#include "glc3-pip-skybox-equirectangular.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/glc3-eng-engine.hpp"
#include "../shader/glc3-shd-skybox-equirectangular.hpp"
#include "glc3-pip-skybox-equirectangular-resource-set.hpp"

gearoenix::glc3::pipeline::SkyboxEquirectangular::SkyboxEquirectangular(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::SkyboxEquirectangular(e, c)
    , shd(new shader::SkyboxEquirectangular(e, c))
{
}

gearoenix::glc3::pipeline::SkyboxEquirectangular::~SkyboxEquirectangular() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::glc3::pipeline::SkyboxEquirectangular::create_resource_set() const noexcept
{
    return new SkyboxEquirectangularResourceSet(shd, shared_from_this());
}

#endif
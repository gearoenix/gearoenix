#include "gx-gles2-pip-skybox-equirectangular.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gx-gles2-eng-engine.hpp"
#include "../shader/gx-gles2-shd-skybox-equirectangular.hpp"
#include "gx-gles2-pip-skybox-equirectangular-resource-set.hpp"

gearoenix::gles2::pipeline::SkyboxEquirectangular::SkyboxEquirectangular(
    engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::SkyboxEquirectangular(e, c)
    , shd(new shader::SkyboxEquirectangular(e, c))
{
}

std::shared_ptr<gearoenix::gles2::pipeline::SkyboxEquirectangular> gearoenix::gles2::pipeline::SkyboxEquirectangular::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<SkyboxEquirectangular> p(new SkyboxEquirectangular(e, c));
    p->self = p;
    return p;
}

gearoenix::gles2::pipeline::SkyboxEquirectangular::~SkyboxEquirectangular() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::gles2::pipeline::SkyboxEquirectangular::create_resource_set() const noexcept
{
    return new SkyboxEquirectangularResourceSet(shd, self.lock());
}

#endif
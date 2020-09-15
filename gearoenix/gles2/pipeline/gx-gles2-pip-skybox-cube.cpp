#include "gx-gles2-pip-skybox-cube.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gx-gles2-eng-engine.hpp"
#include "../shader/gx-gles2-shd-skybox-cube.hpp"
#include "gx-gles2-pip-skybox-cube-resource-set.hpp"

gearoenix::gles2::pipeline::SkyboxCube::SkyboxCube(
    engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::SkyboxCube(e, c)
    , shd(new shader::SkyboxCube(e, c))
{
}

std::shared_ptr<gearoenix::gles2::pipeline::SkyboxCube> gearoenix::gles2::pipeline::SkyboxCube::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<SkyboxCube> p(new SkyboxCube(e, c));
    p->self = p;
    return p;
}

gearoenix::gles2::pipeline::SkyboxCube::~SkyboxCube() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::gles2::pipeline::SkyboxCube::create_resource_set() const noexcept
{
    return new SkyboxCubeResourceSet(shd, self.lock());
}

#endif
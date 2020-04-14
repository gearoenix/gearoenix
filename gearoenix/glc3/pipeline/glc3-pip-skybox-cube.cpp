#include "glc3-pip-skybox-cube.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/glc3-eng-engine.hpp"
#include "../shader/glc3-shd-skybox-cube.hpp"
#include "glc3-pip-skybox-cube-resource-set.hpp"

gearoenix::glc3::pipeline::SkyboxCube::SkyboxCube(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::SkyboxCube(e, c)
    , shd(new shader::SkyboxCube(e, c))
{
}

std::shared_ptr<gearoenix::glc3::pipeline::SkyboxCube> gearoenix::glc3::pipeline::SkyboxCube::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<SkyboxCube> p(new SkyboxCube(e, c));
    p->self = p;
    return p;
}

gearoenix::glc3::pipeline::SkyboxCube::~SkyboxCube() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::glc3::pipeline::SkyboxCube::create_resource_set() const noexcept
{
    return new SkyboxCubeResourceSet(shd, self.lock());
}

#endif
#include "gles3-pip-shadow-mapper.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../engine/gles3-eng-engine.hpp"
#include "../shader/gles3-shd-shadow-mapper.hpp"
#include "gles3-pip-shadow-mapper-resource-set.hpp"

gearoenix::gles3::pipeline::ShadowMapper::ShadowMapper(engine::Engine*const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::ShadowMapper(e, c)
    , shd(new shader::ShadowMapper(e, c))
{
}

gearoenix::render::pipeline::ResourceSet* gearoenix::gles3::pipeline::ShadowMapper::create_resource_set() const noexcept
{
    return new ShadowMapperResourceSet(shd);
}
#endif
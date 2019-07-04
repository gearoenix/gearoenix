#include "gles2-pip-shadow-mapper.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gles2-eng-engine.hpp"
#include "../shader/gles2-shd-shadow-mapper.hpp"
#include "gles2-pip-shadow-mapper-resource-set.hpp"

gearoenix::gles2::pipeline::ShadowMapper::ShadowMapper(engine::Engine*const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::ShadowMapper(e, c)
    , shd(new shader::ShadowMapper(e, c))
{
}

gearoenix::render::pipeline::ResourceSet* gearoenix::gles2::pipeline::ShadowMapper::create_resource_set() const noexcept
{
    return new ShadowMapperResourceSet(shd);
}
#endif
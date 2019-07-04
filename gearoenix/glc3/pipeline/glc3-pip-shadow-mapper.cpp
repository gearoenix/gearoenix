#include "glc3-pip-shadow-mapper.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/glc3-eng-engine.hpp"
#include "../shader/glc3-shd-shadow-mapper.hpp"
#include "glc3-pip-shadow-mapper-resource-set.hpp"

gearoenix::glc3::pipeline::ShadowMapper::ShadowMapper(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::ShadowMapper(e, c)
    , shd(new shader::ShadowMapper(e, c))
{
}

gearoenix::render::pipeline::ResourceSet* gearoenix::glc3::pipeline::ShadowMapper::create_resource_set() const noexcept
{
    return new ShadowMapperResourceSet(shd);
}
#endif
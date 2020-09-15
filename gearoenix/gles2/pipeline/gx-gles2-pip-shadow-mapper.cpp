#include "gx-gles2-pip-shadow-mapper.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gx-gles2-eng-engine.hpp"
#include "../shader/gx-gles2-shd-shadow-mapper.hpp"
#include "gx-gles2-pip-shadow-mapper-resource-set.hpp"

gearoenix::gles2::pipeline::ShadowMapper::ShadowMapper(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::ShadowMapper(e, c)
    , shd(new shader::ShadowMapper(e, c))
{
}

std::shared_ptr<gearoenix::gles2::pipeline::ShadowMapper> gearoenix::gles2::pipeline::ShadowMapper::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<ShadowMapper> p(new ShadowMapper(e, c));
    p->self = p;
    return p;
}

gearoenix::gles2::pipeline::ShadowMapper::~ShadowMapper() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::gles2::pipeline::ShadowMapper::create_resource_set() const noexcept
{
    return new ShadowMapperResourceSet(shd, self.lock());
}

#endif
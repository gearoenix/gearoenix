#include "gx-glc3-pip-unlit.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/gx-glc3-eng-engine.hpp"
#include "../shader/gx-glc3-shd-unlit.hpp"
#include "gx-glc3-pip-unlit-resource-set.hpp"

gearoenix::glc3::pipeline::Unlit::Unlit(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::Unlit(e, c)
    , shd(new shader::Unlit(e, c))
{
}

std::shared_ptr<gearoenix::glc3::pipeline::Unlit> gearoenix::glc3::pipeline::Unlit::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<Unlit> p(new Unlit(e, c));
    p->self = p;
    return p;
}

gearoenix::glc3::pipeline::Unlit::~Unlit() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::glc3::pipeline::Unlit::create_resource_set() const noexcept
{
    return new UnlitResourceSet(shd, self.lock());
}

#endif
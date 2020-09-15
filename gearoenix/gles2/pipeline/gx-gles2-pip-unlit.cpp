#include "gx-gles2-pip-unlit.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gx-gles2-eng-engine.hpp"
#include "../shader/gx-gles2-shd-unlit.hpp"
#include "gx-gles2-pip-unlit-resource-set.hpp"

gearoenix::gles2::pipeline::Unlit::Unlit(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::Unlit(e, c)
    , shd(new shader::Unlit(e, c))
{
}

std::shared_ptr<gearoenix::gles2::pipeline::Unlit> gearoenix::gles2::pipeline::Unlit::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<Unlit> p(new Unlit(e, c));
    p->self = p;
    return p;
}

gearoenix::gles2::pipeline::Unlit::~Unlit() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::gles2::pipeline::Unlit::create_resource_set() const noexcept
{
    return new UnlitResourceSet(shd, self.lock());
}

#endif
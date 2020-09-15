#include "gx-glc3-pip-forward-pbr.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/gx-glc3-eng-engine.hpp"
#include "../shader/gx-glc3-shd-forward-pbr.hpp"
#include "gx-glc3-pip-forward-pbr-resource-set.hpp"

gearoenix::glc3::pipeline::ForwardPbr::ForwardPbr(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::ForwardPbr(e, c)
    , shd(new shader::ForwardPbr(e, c))
{
}

std::shared_ptr<gearoenix::glc3::pipeline::ForwardPbr> gearoenix::glc3::pipeline::ForwardPbr::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    std::shared_ptr<ForwardPbr> p(new ForwardPbr(e, c));
    p->self = p;
    return p;
}

gearoenix::glc3::pipeline::ForwardPbr::~ForwardPbr() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::glc3::pipeline::ForwardPbr::create_resource_set() const noexcept
{
    return new ForwardPbrResourceSet(shd, self.lock());
}

#endif
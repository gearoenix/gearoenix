#include "gles2-pip-forward-pbr.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gles2-eng-engine.hpp"
#include "../shader/gles2-shd-forward-pbr.hpp"
#include "gles2-pip-forward-pbr-resource-set.hpp"

gearoenix::gles2::pipeline::ForwardPbr::ForwardPbr(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::ForwardPbr(e, c)
    , shd(new shader::ForwardPbr(e, c))
{
}

std::shared_ptr<gearoenix::gles2::pipeline::ForwardPbr> gearoenix::gles2::pipeline::ForwardPbr::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<ForwardPbr> p(new ForwardPbr(e, c));
    p->self = p;
    return p;
}

gearoenix::gles2::pipeline::ForwardPbr::~ForwardPbr() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::gles2::pipeline::ForwardPbr::create_resource_set() const noexcept
{
    return new ForwardPbrResourceSet(shd, self.lock());
}

#endif
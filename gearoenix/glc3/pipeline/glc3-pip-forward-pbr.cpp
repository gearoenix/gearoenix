#include "glc3-pip-forward-pbr.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/glc3-eng-engine.hpp"
#include "../shader/glc3-shd-effect-forward-pbr.hpp"
#include "glc3-pip-forward-pbr-resource-set.hpp"

gearoenix::glc3::pipeline::ForwardPbr::ForwardPbr(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::ForwardPbr(e, c)
    , shd(new shader::ForwardPbr(e, c))
{
}

gearoenix::render::pipeline::ResourceSet* gearoenix::glc3::pipeline::ForwardPbr::create_resource_set() const noexcept
{
    return new ForwardPbrResourceSet(shd);
}
#endif
#include "gles3-pip-forward-pbr-directional-shadow.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../engine/gles3-eng-engine.hpp"
#include "../shader/gles3-shd-effect-forward-pbr-directional-shadow.hpp"
#include "gles3-pip-forward-pbr-directional-shadow-resource-set.hpp"

gearoenix::gles3::pipeline::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::ForwardPbrDirectionalShadow(e, c)
    , shd(new shader::ForwardPbrDirectionalShadow(e, c))
{
}

gearoenix::render::pipeline::ResourceSet* gearoenix::gles3::pipeline::ForwardPbrDirectionalShadow::create_resource_set() const noexcept
{
    return new ForwardPbrDirectionalShadowResourceSet(shd);
}
#endif
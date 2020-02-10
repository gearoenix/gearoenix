#include "gles2-pip-unlit.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gles2-eng-engine.hpp"
#include "../shader/gles2-shd-unlit.hpp"
#include "gles2-pip-unlit-resource-set.hpp"

gearoenix::gles2::pipeline::Unlit::Unlit(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::Unlit(e, c)
    , shd(new shader::Unlit(e, c))
{
}

gearoenix::gles2::pipeline::Unlit::~Unlit() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::gles2::pipeline::Unlit::create_resource_set() const noexcept
{
    return new UnlitResourceSet(shd, shared_from_this());
}

#endif
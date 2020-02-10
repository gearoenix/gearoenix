#include "glc3-pip-unlit.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/glc3-eng-engine.hpp"
#include "../shader/glc3-shd-unlit.hpp"
#include "glc3-pip-unlit-resource-set.hpp"

gearoenix::glc3::pipeline::Unlit::Unlit(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::Unlit(e, c)
    , shd(new shader::Unlit(e, c))
{
}

gearoenix::glc3::pipeline::Unlit::~Unlit() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::glc3::pipeline::Unlit::create_resource_set() const noexcept
{
    return new UnlitResourceSet(shd, shared_from_this());
}

#endif
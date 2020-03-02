#include "gles2-pip-irradiance-convoluter.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gles2-eng-engine.hpp"
#include "../shader/gles2-shd-irradiance-convoluter.hpp"
#include "gles2-pip-irradiance-convoluter-resource-set.hpp"

gearoenix::gles2::pipeline::IrradianceConvoluter::IrradianceConvoluter(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::IrradianceConvoluter(e, c)
    , shd(new shader::IrradianceConvoluter(e, c))
{
}

std::shared_ptr<gearoenix::gles2::pipeline::IrradianceConvoluter> gearoenix::gles2::pipeline::IrradianceConvoluter::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<IrradianceConvoluter> p(new IrradianceConvoluter(e, c));
    p->self = p;
    return p;
}

gearoenix::gles2::pipeline::IrradianceConvoluter::~IrradianceConvoluter() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::gles2::pipeline::IrradianceConvoluter::create_resource_set() const noexcept
{
    return new IrradianceConvoluterResourceSet(shd, self.lock());
}

#endif
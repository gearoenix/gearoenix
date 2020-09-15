#include "gx-glc3-pip-irradiance-convoluter.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/gx-glc3-eng-engine.hpp"
#include "../shader/gx-glc3-shd-irradiance-convoluter.hpp"
#include "gx-glc3-pip-irradiance-convoluter-resource-set.hpp"

gearoenix::glc3::pipeline::IrradianceConvoluter::IrradianceConvoluter(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::IrradianceConvoluter(e, c)
    , shd(new shader::IrradianceConvoluter(e, c))
{
}

std::shared_ptr<gearoenix::glc3::pipeline::IrradianceConvoluter> gearoenix::glc3::pipeline::IrradianceConvoluter::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<IrradianceConvoluter> p(new IrradianceConvoluter(e, c));
    p->self = p;
    return p;
}

gearoenix::glc3::pipeline::IrradianceConvoluter::~IrradianceConvoluter() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::glc3::pipeline::IrradianceConvoluter::create_resource_set() const noexcept
{
    return new IrradianceConvoluterResourceSet(shd, self.lock());
}

#endif
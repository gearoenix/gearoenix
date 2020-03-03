#include "glc3-pip-radiance-convoluter.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/glc3-eng-engine.hpp"
#include "../shader/glc3-shd-radiance-convoluter.hpp"
#include "glc3-pip-radiance-convoluter-resource-set.hpp"

gearoenix::glc3::pipeline::RadianceConvoluter::RadianceConvoluter(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : render::pipeline::RadianceConvoluter(e, c)
    , shd(new shader::RadianceConvoluter(e, c))
{
}

std::shared_ptr<gearoenix::glc3::pipeline::RadianceConvoluter> gearoenix::glc3::pipeline::RadianceConvoluter::construct(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    const std::shared_ptr<RadianceConvoluter> p(new RadianceConvoluter(e, c));
    p->self = p;
    return p;
}

gearoenix::glc3::pipeline::RadianceConvoluter::~RadianceConvoluter() noexcept = default;

gearoenix::render::pipeline::ResourceSet* gearoenix::glc3::pipeline::RadianceConvoluter::create_resource_set() const noexcept
{
    return new RadianceConvoluterResourceSet(shd, self.lock());
}

#endif
#include "rnd-pip-type.hpp"

std::string std::to_string(const gearoenix::render::pipeline::Type t) noexcept
{
    switch (t) {
    case gearoenix::render::pipeline::Type::DeferredPbr:
        return "DeferredPbr";
    case gearoenix::render::pipeline::Type::ForwardPbr:
        return "ForwardPbr";
    case gearoenix::render::pipeline::Type::GBufferFiller:
        return "GBufferFiller";
    case gearoenix::render::pipeline::Type::LightBlooming:
        return "LightBlooming";
    case gearoenix::render::pipeline::Type::ShadowAccumulator:
        return "ShadowAccumulator";
    case gearoenix::render::pipeline::Type::ShadowMapper:
        return "ShadowMapper";
    case gearoenix::render::pipeline::Type::SSAO:
        return "SSAO";
    case gearoenix::render::pipeline::Type::SSR:
        return "SSR";
    case gearoenix::render::pipeline::Type::Unlit:
        return "Unlit";
    case gearoenix::render::pipeline::Type::SkyboxCube:
        return "SkyboxCube";
    case gearoenix::render::pipeline::Type::SkyboxEquirectangular:
        return "SkyboxEquirectangular";
    case gearoenix::render::pipeline::Type::IrradianceConvoluter:
        return "IrradianceConvoluter";
    case gearoenix::render::pipeline::Type::RadianceConvoluter:
        return "RadianceConvoluter";
    }
}
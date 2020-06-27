#ifndef GEAROENIX_RENDER_PIPELINE_TYPE_HPP
#define GEAROENIX_RENDER_PIPELINE_TYPE_HPP

#include <string>

namespace gearoenix::render::pipeline {
enum struct Type : unsigned int {
    DeferredPbr = 1,
    ForwardPbr = 2,
    GBufferFiller = 3,
    LightBlooming = 4,
    ShadowAccumulator = 5,
    ShadowMapper = 6,
    SSAO = 7,
    SSR = 8,
    Unlit = 9,
    SkyboxCube = 10,
    SkyboxEquirectangular = 11,
    IrradianceConvoluter = 12,
    RadianceConvoluter = 13,
};
}

namespace std {
[[nodiscard]] string to_string(const gearoenix::render::pipeline::Type t)
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
}
#endif

#include "glc3-pip-manager.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-pip-forward-pbr.hpp"
#include "glc3-pip-irradiance-convoluter.hpp"
#include "glc3-pip-radiance-convoluter.hpp"
#include "glc3-pip-shadow-mapper.hpp"
#include "glc3-pip-skybox-equirectangular.hpp"
#include "glc3-pip-unlit.hpp"

gearoenix::glc3::pipeline::Manager::Manager(engine::Engine* const engine) noexcept
    : render::pipeline::Manager(engine)
{
}

gearoenix::glc3::pipeline::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::glc3::pipeline::Manager::get(
    const render::pipeline::Type pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline>& end) noexcept
{
    const std::shared_ptr<render::pipeline::Pipeline> p = pipelines.get<render::pipeline::Pipeline>(pipeline_type_id,
        [this, pipeline_type_id, end]() noexcept -> std::shared_ptr<render::pipeline::Pipeline> {
            auto* const eng = reinterpret_cast<engine::Engine*>(e);
            const core::sync::EndCaller<core::sync::EndCallerIgnore> call([end] {});
            switch (pipeline_type_id) {
            case render::pipeline::Type::ForwardPbr:
                return ForwardPbr::construct(eng, call);
            case render::pipeline::Type::ShadowMapper:
                return ShadowMapper::construct(eng, call);
            case render::pipeline::Type::Unlit:
                return Unlit::construct(eng, call);
            case render::pipeline::Type::SkyboxEquirectangular:
                return SkyboxEquirectangular::construct(eng, call);
            case render::pipeline::Type::IrradianceConvoluter:
                return IrradianceConvoluter::construct(eng, call);
            case render::pipeline::Type::RadianceConvoluter:
                return RadianceConvoluter::construct(eng, call);
            default:
                GXLOGF("Unexpected pipeline type: " << static_cast<unsigned int>(pipeline_type_id))
            }
        });
    end.set_data(p);
    return p;
}

#endif

#include "gx-gles2-pip-manager.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gx-gles2-eng-engine.hpp"
#include "gx-gles2-pip-forward-pbr.hpp"
#include "gx-gles2-pip-irradiance-convoluter.hpp"
#include "gx-gles2-pip-shadow-mapper.hpp"
#include "gx-gles2-pip-skybox-cube.hpp"
#include "gx-gles2-pip-skybox-equirectangular.hpp"
#include "gx-gles2-pip-unlit.hpp"

gearoenix::gles2::pipeline::Manager::Manager(engine::Engine* const engine) noexcept
    : render::pipeline::Manager(engine)
{
}

gearoenix::gles2::pipeline::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::gles2::pipeline::Manager::get(const render::pipeline::Type pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline>& end) noexcept
{
    const std::shared_ptr<render::pipeline::Pipeline> p = pipelines.get<render::pipeline::Pipeline>(pipeline_type_id,
        [this, pipeline_type_id, end]() noexcept -> std::shared_ptr<render::pipeline::Pipeline> {
            auto eng = reinterpret_cast<engine::Engine*>(e);
            const core::sync::EndCaller<core::sync::EndCallerIgnore> c([end] {});
            switch (pipeline_type_id) {
            case render::pipeline::Type::ForwardPbr:
                return ForwardPbr::construct(eng, c);
            case render::pipeline::Type::IrradianceConvoluter:
                return IrradianceConvoluter::construct(eng, c);
            case render::pipeline::Type::ShadowMapper:
                return ShadowMapper::construct(eng, c);
            case render::pipeline::Type::SkyboxCube:
                return SkyboxCube::construct(eng, c);
            case render::pipeline::Type::SkyboxEquirectangular:
                return SkyboxEquirectangular::construct(eng, c);
            case render::pipeline::Type::Unlit:
                return Unlit::construct(eng, c);
            default:
                GXLOGF("Unexpected pipeline type: " << static_cast<unsigned int>(pipeline_type_id))
            }
        });
    end.set_data(p);
    return p;
}

#endif

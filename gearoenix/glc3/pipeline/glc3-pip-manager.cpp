#include "glc3-pip-manager.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/glc3-eng-engine.hpp"
#include "glc3-pip-forward-pbr.hpp"
#include "glc3-pip-shadow-mapper.hpp"

gearoenix::glc3::pipeline::Manager::Manager(engine::Engine* const engine) noexcept
    : render::pipeline::Manager(engine)
{
}

gearoenix::glc3::pipeline::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::glc3::pipeline::Manager::get(const render::pipeline::Type pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline>& end) noexcept
{
    const std::shared_ptr<render::pipeline::Pipeline> p = pipelines.get<render::pipeline::Pipeline>(pipeline_type_id, [this, pipeline_type_id, end] {
        auto* const gles2eng = reinterpret_cast<engine::Engine*>(e);
        switch (pipeline_type_id) {
        case render::pipeline::Type::ForwardPbr:
            return std::shared_ptr<render::pipeline::Pipeline>(new ForwardPbr(gles2eng,
                core::sync::EndCaller<core::sync::EndCallerIgnore>([end] {})));
        case render::pipeline::Type::ShadowMapper:
            return std::shared_ptr<render::pipeline::Pipeline>(new ShadowMapper(gles2eng,
                core::sync::EndCaller<core::sync::EndCallerIgnore>([end] {})));
        default:
            GXLOGF("Unexpected pipeline type: " << static_cast<unsigned int>(pipeline_type_id))
        }
    });
    end.set_data(p);
    return p;
}

#endif

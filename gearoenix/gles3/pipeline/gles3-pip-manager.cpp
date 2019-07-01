#include "gles3-pip-manager.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../buffer/gles3-buf-uniform.hpp"
#include "../engine/gles3-eng-engine.hpp"
#include "gles3-pip-forward-pbr-directional-shadow.hpp"
#include "gles3-pip-shadow-mapper.hpp"

gearoenix::gles3::pipeline::Manager::Manager(engine::Engine*const engine) noexcept
    : render::pipeline::Manager(engine)
{
}

gearoenix::gles3::pipeline::Manager::~Manager() noexcept {}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::gles3::pipeline::Manager::get(const render::pipeline::Type::Id pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline>& end) noexcept
{
    const std::shared_ptr<render::pipeline::Pipeline> p = pipelines.get<render::pipeline::Pipeline>(pipeline_type_id, [this, pipeline_type_id, end] {
        engine::Engine*const gles2eng = reinterpret_cast<engine::Engine*>(e);
        switch (pipeline_type_id) {
        case render::pipeline::Type::ForwardPbrDirectionalShadow:
            return std::shared_ptr<render::pipeline::Pipeline>(new ForwardPbrDirectionalShadow(gles2eng,
                core::sync::EndCaller<core::sync::EndCallerIgnore>([end] {})));
        case render::pipeline::Type::ShadowMapper:
            return std::shared_ptr<render::pipeline::Pipeline>(new ShadowMapper(gles2eng,
                core::sync::EndCaller<core::sync::EndCallerIgnore>([end] {})));
        default:
            GXLOGF("Unexpected pipeline type: " << pipeline_type_id)
            break;
        }
    });
    end.set_data(p);
    return p;
}

#endif

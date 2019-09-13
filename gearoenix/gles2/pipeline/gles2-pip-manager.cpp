#include "gles2-pip-manager.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../buffer/gles2-buf-uniform.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-pip-forward-pbr.hpp"
#include "gles2-pip-shadow-mapper.hpp"

gearoenix::gles2::pipeline::Manager::Manager(engine::Engine* const engine) noexcept
    : render::pipeline::Manager(engine)
{
}

gearoenix::gles2::pipeline::Manager::~Manager() noexcept {}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::gles2::pipeline::Manager::get(const render::pipeline::Type pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline>& end) noexcept
{
    const std::shared_ptr<render::pipeline::Pipeline> p = pipelines.get<render::pipeline::Pipeline>(pipeline_type_id, [this, pipeline_type_id, end] {
        auto gles2eng = reinterpret_cast<engine::Engine*>(e);
        switch (pipeline_type_id) {
        case render::pipeline::Type::ForwardPbr:
            return std::shared_ptr<render::pipeline::Pipeline>(new ForwardPbr(gles2eng,
                core::sync::EndCaller<core::sync::EndCallerIgnore>([end] {})));
        case render::pipeline::Type::ShadowMapper:
            return std::shared_ptr<render::pipeline::Pipeline>(new ShadowMapper(gles2eng,
                core::sync::EndCaller<core::sync::EndCallerIgnore>([end] {})));
        default:
            GXLOGF("Unexpected pipeline type: " << static_cast<unsigned int>(pipeline_type_id));
            break;
        }
    });
    end.set_data(p);
    return p;
}

#endif

#include "gx-glc3-pip-pipeline.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../system/gx-sys-app.hpp"
#include "../../system/gx-sys-log.hpp"
#include "../engine/gx-glc3-eng-engine.hpp"
#include "../shader/gx-glc3-shd-shader.hpp"

gearoenix::glc3::pipeline::Pipeline::Pipeline(
    const render::pipeline::Type::Id pid,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call)
    : render::pipeline::Pipeline(pid, e, call)
{
}

gearoenix::glc3::pipeline::Pipeline::~Pipeline()
{
}

void gearoenix::glc3::pipeline::Pipeline::bind()
{
    GX_UNIMPLEMENTED;
}

#endif

#include "gles3-pip-pipeline.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles3-eng-engine.hpp"
#include "../shader/gles3-shd-shader.hpp"

gearoenix::gles3::pipeline::Pipeline::Pipeline(
    const render::pipeline::Type::Id pid,
    engine::Engine*const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call)
    : render::pipeline::Pipeline(pid, e, call)
{
}

gearoenix::gles3::pipeline::Pipeline::~Pipeline()
{
}

void gearoenix::gles3::pipeline::Pipeline::bind()
{
    GXUNIMPLEMENTED;
}

#endif

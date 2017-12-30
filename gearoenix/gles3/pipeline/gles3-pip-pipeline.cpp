#include "gles3-pip-pipeline.hpp"
#ifdef USE_OPENGL_ES3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../gles3-engine.hpp"
#include "../shader/gles3-shader.hpp"

gearoenix::gles3::pipeline::Pipeline::Pipeline(core::Id sid, Engine* eng, core::EndCaller<core::EndCallerIgnore> call)
    : render::pipeline::Pipeline(sid, eng, call)
{
}

gearoenix::gles3::pipeline::Pipeline::~Pipeline()
{
}

void gearoenix::gles3::pipeline::Pipeline::bind()
{
    shd->use();
    GXLOGE("It can be better.");
}

#endif

#include "gles2-pip-pipeline.hpp"
#ifdef USE_OPENGL_ES2
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../gles2-engine.hpp"
#include "../shader/gles2-shader.hpp"

gearoenix::gles2::pipeline::Pipeline::Pipeline(core::Id sid, Engine* eng, std::shared_ptr<core::EndCaller> call)
    : render::pipeline::Pipeline(sid, eng, call)
{
}

gearoenix::gles2::pipeline::Pipeline::~Pipeline()
{
}

void gearoenix::gles2::pipeline::Pipeline::bind()
{
    shd->use();
    GXLOGE("It can be better.");
}

#endif

#include "gx-dx11-pip-pipeline.hpp"
#ifdef USE_DIRECTX11
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../system/gx-sys-app.hpp"
#include "../../system/gx-sys-log.hpp"
#include "../dx11-engine.hpp"
#include "../shader/dx11-shader.hpp"

gearoenix::dx11::pipeline::Pipeline::Pipeline(
    core::Id sid, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> call)
    : render::pipeline::Pipeline(sid, eng, call)
{
}

gearoenix::dx11::pipeline::Pipeline::~Pipeline()
{
}

void gearoenix::dx11::pipeline::Pipeline::bind()
{
    shd->use();
    GXTODO;
}

#endif

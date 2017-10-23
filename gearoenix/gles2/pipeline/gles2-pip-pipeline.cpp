#include "gles2-pip-pipeline.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
gearoenix::gles2::pipeline::Pipeline::Pipeline(core::Id sid, Engine* eng)
    : render::pipeline::Pipeline(sid, reinterpret_cast<render::Engine*>(eng))
{
    TODO;
}

gearoenix::gles2::pipeline::Pipeline::~Pipeline()
{
    TODO;
}

void gearoenix::render::pipeline::Pipeline::bind()
{
    UNIMPLEMENTED;
}

#endif

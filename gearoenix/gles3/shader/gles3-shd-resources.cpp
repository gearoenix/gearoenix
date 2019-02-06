#include "gles3-shd-resources.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/sys-log.hpp"

#include "../buffer/gles3-buf-uniform.hpp"
#include "../gles3-engine.hpp"
#include "../pipeline/gles3-pip-pipeline.hpp"

gearoenix::gles3::shader::Resources::Resources(Engine* e, pipeline::Pipeline* p, buffer::Uniform* u)
    : render::shader::Resources(e, p, u)
{
}

gearoenix::gles3::shader::Resources::~Resources()
{
}

void gearoenix::gles3::shader::Resources::bind()
{
    GXUNIMPLEMENTED;
}

#endif

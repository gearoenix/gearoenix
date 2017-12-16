#include "gles2-shd-resources.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"

#include "../buffer/gles2-buf-uniform.hpp"
#include "../gles2-engine.hpp"
#include "../pipeline/gles2-pip-pipeline.hpp"

gearoenix::gles2::shader::Resources::Resources(Engine* e, pipeline::Pipeline* p, buffer::Uniform* u)
    : render::shader::Resources(e, p, u)
{
}

gearoenix::gles2::shader::Resources::~Resources()
{
}

void gearoenix::gles2::shader::Resources::bind()
{
    UNIMPLEMENTED;
}

#endif

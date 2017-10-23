#include "gles2-shd-resources.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
gearoenix::gles2::shader::Resources::Resources(Engine* e, pipeline::Pipeline* p, buffer::Uniform* u)
    : render::shader::Resources(e, p, u)
{
}

gearoenix::gles2::shader::Resources::Resources(Engine* e, pipeline::Pipeline* p, buffer::Uniform* u, texture::Texture2D* t)
    : render::shader::Resources(e, p, u, t)
{
}

gearoenix::gles2::shader::Resources::~Resources()
{
}

void gearoenix::gles2::shader::Resources::bind(pipeline::Pipeline*)
{
    UNIMPLEMENTED;
}

#endif

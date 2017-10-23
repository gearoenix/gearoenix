#include "gles2-buf-uniform.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"

gearoenix::gles2::buffer::Uniform::Uniform(Engine* eng)
    : render::buffer::Uniform(reinterpret_cast<render::Engine*>(eng))
{
    UNIMPLEMENTED;
}

gearoenix::gles2::buffer::Uniform::~Uniform()
{
    UNIMPLEMENTED;
}

void gearoenix::gles2::buffer::Uniform::update(const void*, unsigned int)
{
    UNIMPLEMENTED;
}

#endif

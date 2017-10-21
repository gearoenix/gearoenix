#include "gles2-buf-uniform.hpp"
#ifdef USE_OPENGL_ES2
#include "../../system/sys-log.hpp"

gearoenix::render::buffer::Uniform::Uniform(unsigned int, Engine*)
{
    TODO;
}

gearoenix::render::buffer::Uniform::~Uniform()
{
    TODO;
}

const gearoenix::render::buffer::SubBuffer* const* gearoenix::render::buffer::Uniform::get_vbuf() const
{
    TODO;
    return nullptr;
}

const gearoenix::render::buffer::SubBuffer* const* gearoenix::render::buffer::Uniform::get_cbuf() const
{
    TODO;
    return nullptr;
}

unsigned int gearoenix::render::buffer::Uniform::get_count() const
{
    TODO;
    return 0;
}

unsigned int gearoenix::render::buffer::Uniform::get_size() const
{
    TODO;
    return 0;
}

void gearoenix::render::buffer::Uniform::update(const void*, unsigned int)
{
    TODO;
}

#endif

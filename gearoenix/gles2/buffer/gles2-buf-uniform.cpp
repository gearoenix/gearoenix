#include "gles2-buf-uniform.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../gles2-engine.hpp"
#include <cstring>

gearoenix::gles2::buffer::Uniform::Uniform(unsigned int s, Engine* eng)
    : render::buffer::Uniform(eng)
    , data(new unsigned char[s])
{
}

gearoenix::gles2::buffer::Uniform::~Uniform()
{
    delete[] data;
}

void gearoenix::gles2::buffer::Uniform::update(const void* src, unsigned int size)
{
    std::memcpy(data, src, size);
}

void* gearoenix::gles2::buffer::Uniform::get_data()
{
    return data;
}

#endif

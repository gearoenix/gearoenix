#include "gles3-buf-uniform.hpp"
#ifdef USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "../gles3-engine.hpp"
#include <cstring>

gearoenix::gles3::buffer::Uniform::Uniform(unsigned int s, Engine* eng)
    : render::buffer::Uniform(eng)
    , data(new unsigned char[s])
{
}

gearoenix::gles3::buffer::Uniform::~Uniform()
{
    delete[] data;
}

void gearoenix::gles3::buffer::Uniform::update(const void* src, unsigned int size)
{
    std::memcpy(data, src, size);
}

void* gearoenix::gles3::buffer::Uniform::get_data()
{
    return data;
}

#endif

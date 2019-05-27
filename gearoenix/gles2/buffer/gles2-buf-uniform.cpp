#include "gles2-buf-uniform.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include <cstring>

gearoenix::gles2::buffer::Uniform::Uniform(const unsigned int s, engine::Engine* const e) noexcept
    : render::buffer::Uniform(s, e)
    , data(new unsigned char[s])
{
}

gearoenix::gles2::buffer::Uniform::~Uniform() noexcept
{
    delete[] data;
    data = nullptr;
}

void gearoenix::gles2::buffer::Uniform::update(const void*const src) noexcept
{
    std::memcpy(data, src, size);
}

const void* gearoenix::gles2::buffer::Uniform::get_data() noexcept
{
    return data;
}

#endif

#include "gx-gles2-buf-uniform.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/gx-sys-log.hpp"
#include "../engine/gx-gles2-eng-engine.hpp"
#include <cstring>

const void* gearoenix::gles2::buffer::Uniform::get_data() const noexcept
{
    return data.data();
}

void* gearoenix::gles2::buffer::Uniform::get_data() noexcept
{
    return data.data();
}

void gearoenix::gles2::buffer::Uniform::update(const void* const src) noexcept
{
    std::memcpy(data.data(), src, buffer_size);
}

gearoenix::gles2::buffer::Uniform::Uniform(const std::size_t s, engine::Engine* const e) noexcept
    : render::buffer::Uniform(s, e)
    , data(s)
{
}

gearoenix::gles2::buffer::Uniform::~Uniform() noexcept
{
    data.clear();
}

#endif

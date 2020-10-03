#include "gx-glc3-buf-uniform.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/gx-glc3-eng-engine.hpp"
#include <cstring>

gearoenix::glc3::buffer::Uniform::Uniform(const std::size_t s, const std::size_t fn, engine::Engine* const e) noexcept
    : render::buffer::Uniform(s, fn, e)
    , data(s)
{
}

void gearoenix::glc3::buffer::Uniform::update(const void* const src) noexcept
{
    std::memcpy(data.data(), src, size);
}

const void* gearoenix::glc3::buffer::Uniform::get_data() const noexcept
{
    return data.data();
}

void* gearoenix::glc3::buffer::Uniform::get_data() noexcept
{
    return data.data();
}

#endif

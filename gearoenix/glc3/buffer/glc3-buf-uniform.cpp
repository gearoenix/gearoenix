#include "glc3-buf-uniform.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../system/sys-log.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include <cstring>

gearoenix::glc3::buffer::Uniform::Uniform(const std::size_t s, engine::Engine* const e) noexcept
    : render::buffer::Uniform(s, e)
{
    data.resize(s);
}

gearoenix::glc3::buffer::Uniform::~Uniform()
{
    data.clear();
}

void gearoenix::glc3::buffer::Uniform::update(const void* const src) noexcept
{
    std::memcpy(data.data(), src, buffer_size);
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

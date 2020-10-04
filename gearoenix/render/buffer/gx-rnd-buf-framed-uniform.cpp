#include "gx-rnd-buf-framed-uniform.hpp"
#include "gx-rnd-buf-manager.hpp"

gearoenix::render::buffer::FramedUniform::FramedUniform(const std::size_t s, engine::Engine* const e) noexcept
    : e(e)
    , uniforms(e->get_frames_count())
{
    auto* const buff_mgr = e->get_buffer_manager();
    const auto fc = static_cast<std::size_t>(e->get_frames_count());
    for (std::size_t i = 0; i < fc; ++i)
        uniforms[i] = buff_mgr->create_uniform(s, i);
}

gearoenix::render::buffer::FramedUniform::~FramedUniform() noexcept
{
    uniforms.clear();
}

const gearoenix::render::buffer::Uniform* gearoenix::render::buffer::FramedUniform::get_buffer() const noexcept
{
    return uniforms[e->get_frame_number()].get();
}

gearoenix::render::buffer::Uniform* gearoenix::render::buffer::FramedUniform::get_buffer() noexcept
{
    return uniforms[e->get_frame_number()].get();
}

#include "rnd-buf-framed-uniform.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-buf-manager.hpp"
#include "rnd-buf-uniform.hpp"

gearoenix::render::buffer::FramedUniform::FramedUniform(const std::size_t s, engine::Engine* const e) noexcept
    : e(e)
    , uniforms(e->get_frames_count())
{
    auto* cmd_mgr = e->get_buffer_manager();
    const unsigned int fc = e->get_frames_count();
    for (unsigned int i = 0; i < fc; ++i)
        uniforms[i] = std::unique_ptr<Uniform>(cmd_mgr->create_uniform(s));
}

gearoenix::render::buffer::FramedUniform::~FramedUniform() noexcept
{
}

void gearoenix::render::buffer::FramedUniform::update(const void* data) noexcept
{
    uniforms[e->get_frame_number()]->update(data);
}

const gearoenix::render::buffer::Uniform* gearoenix::render::buffer::FramedUniform::get_buffer() const noexcept
{
    return uniforms[e->get_frame_number()].get();
}

gearoenix::render::buffer::Uniform* gearoenix::render::buffer::FramedUniform::get_buffer() noexcept
{
    return uniforms[e->get_frame_number()].get();
}

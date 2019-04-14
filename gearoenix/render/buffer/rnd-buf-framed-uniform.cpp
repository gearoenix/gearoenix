#include "rnd-buf-framed-uniform.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-buf-manager.hpp"
#include "rnd-buf-uniform.hpp"

static std::vector<std::shared_ptr<gearoenix::render::buffer::Uniform>> create_uniforms(const unsigned int s, const std::shared_ptr<gearoenix::render::engine::Engine>& e)
{
    const unsigned int frames_count = e->get_frames_count();
    std::vector<std::shared_ptr<gearoenix::render::buffer::Uniform>> uniforms(frames_count);
    for (unsigned int i = 0; i < frames_count; ++i)
        uniforms[i] = e->get_buffer_manager()->create_uniform(s);
    return uniforms;
}

gearoenix::render::buffer::FramedUniform::FramedUniform(const unsigned int s, const std::shared_ptr<engine::Engine>& e)
    : e(e)
    , uniforms(create_uniforms(s, e))
{
}

gearoenix::render::buffer::FramedUniform::~FramedUniform()
{
}

void gearoenix::render::buffer::FramedUniform::update(const void* data)
{
    uniforms[e->get_frame_number()]->update(data);
}

const std::shared_ptr<gearoenix::render::buffer::Uniform>& gearoenix::render::buffer::FramedUniform::get_buffer() const
{
    return uniforms[e->get_frame_number()];
}

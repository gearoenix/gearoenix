#include "rnd-lt-directional.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"

gearoenix::render::light::Directional::Directional(const core::Id my_id, system::stream::Stream* const f, engine::Engine* const e) noexcept
    : Light(my_id, f, e)
{
	f->read(uniform.direction[0]);
	f->read(uniform.direction[1]);
	f->read(uniform.direction[2]);
	uniform_buffers = std::make_unique<buffer::FramedUniform>(sizeof(DirectionalUniform), e);
}


gearoenix::render::light::Directional::Directional(const core::Id my_id, engine::Engine* const e) noexcept
    : Light(my_id, e)
{
	uniform_buffers = std::make_unique<buffer::FramedUniform>(sizeof(DirectionalUniform), e);
}

const gearoenix::math::Vec3& gearoenix::render::light::Directional::get_direction() const noexcept
{
    return uniform.direction;
}

void gearoenix::render::light::Directional::set_direction(const gearoenix::math::Vec3& dir) noexcept
{
    uniform.direction = dir;
}

void gearoenix::render::light::Directional::update_uniform() noexcept
{
	uniform_buffers->update(&uniform);
}

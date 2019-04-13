#include "rnd-lt-directional.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::render::light::Directional::Directional(const core::Id my_id, const std::shared_ptr<system::stream::Stream>& f, const std::shared_ptr<engine::Engine>& e)
	: Light(my_id, f, e)
    , direction(f->read<core::Real>(), f->read<core::Real>(), f->read<core::Real>(), 0.0f)
{
}

const gearoenix::math::Vec4 & gearoenix::render::light::Directional::get_direction() const
{
	return direction;
}

gearoenix::math::Vec4 & gearoenix::render::light::Directional::get_direction()
{
	return direction;
}

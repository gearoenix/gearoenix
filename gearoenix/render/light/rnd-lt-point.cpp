#include "rnd-lt-point.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::render::light::Point::Point(const core::Id my_id, const std::shared_ptr<system::stream::Stream>& f, const std::shared_ptr<engine::Engine>& e)
    : Light(my_id, f, e)
{
	f->read(position_radius[0]);
	f->read(position_radius[1]);
	f->read(position_radius[2]);
}

gearoenix::render::light::Point::~Point()
{
}

const gearoenix::math::Vec4& gearoenix::render::light::Point::get_position_radius() const
{
    return position_radius;
}

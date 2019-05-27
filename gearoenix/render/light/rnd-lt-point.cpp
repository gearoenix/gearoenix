#include "rnd-lt-point.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::render::light::Point::Point(const core::Id my_id, system::stream::Stream*const f, engine::Engine*const e) noexcept
        : Light(my_id, f, e)
{
    f->read(position_radius[0]);
    f->read(position_radius[1]);
    f->read(position_radius[2]);
}

const gearoenix::math::Vec4& gearoenix::render::light::Point::get_position_radius() const noexcept
{
    return position_radius;
}

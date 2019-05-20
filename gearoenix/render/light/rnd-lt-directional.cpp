#include "rnd-lt-directional.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::render::light::Directional::Directional(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e) noexcept
    : Light(my_id, f, e)
    , direction(f->read<core::Real>(), f->read<core::Real>(), f->read<core::Real>())
{
}

gearoenix::render::light::Directional::Directional(
    const core::Id my_id,
    const std::shared_ptr<engine::Engine>& e) noexcept
    : Light(my_id, e)
{
}

const gearoenix::math::Vec3& gearoenix::render::light::Directional::get_direction() const noexcept
{
    return direction;
}

void gearoenix::render::light::Directional::set_direction(const gearoenix::math::Vec3& dir) noexcept
{
    direction = dir;
}

std::shared_ptr<gearoenix::render::light::CascadeInfo> gearoenix::render::light::Directional::create_cascades_info(const std::vector<math::Vec3[4]>& partitions) const noexcept
{
	return std::shared_ptr<CascadeInfo>();
}

#include "rnd-lt-point.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../core/cr-static.hpp"
#include "../../math/math-sphere.hpp"
#include "../model/rnd-mdl-model.hpp"
#include <cmath>

void gearoenix::render::light::Point::update_influence_area() noexcept
{
    const auto red = color[0];
    const auto grn = color[1];
    const auto blu = color[2];
    auto mxc = GX_MAX(red, grn);
    mxc = GX_MAX(mxc, blu);
    mxc *= 20.371832715762602978417121711682f;
    mxc = std::sqrtf(std::abs(mxc));
    position_max_radius[3] = mxc;
}

gearoenix::render::light::Point::Point(const core::Id my_id, system::stream::Stream* const f, engine::Engine* const e) noexcept
    : Light(my_id, f, e, Type::POINT)
{
    f->read(position_max_radius[0]);
    f->read(position_max_radius[1]);
    f->read(position_max_radius[2]);
    update_influence_area();
}

const gearoenix::math::Vec4& gearoenix::render::light::Point::get_position_max_radius() const noexcept
{
    return position_max_radius;
}

const gearoenix::core::Real gearoenix::render::light::Point::get_min_radius() const noexcept
{
    return min_radius;
}

bool gearoenix::render::light::Point::is_in_light(const model::Model* m) const noexcept
{
    return math::Sphere(position_max_radius.xyz(), position_max_radius[3]).check_intersection(m->get_occlusion_sphere());
}

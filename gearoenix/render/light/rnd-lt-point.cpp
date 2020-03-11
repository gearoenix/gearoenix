#include "rnd-lt-point.hpp"
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
    mxc *= 20.371832715762602978417121711682;
    mxc = std::sqrt(std::abs(mxc));
    position_max_radius.w = mxc;
}

gearoenix::render::light::Point::Point(const core::Id my_id, system::stream::Stream* const f, engine::Engine* const e) noexcept
    : Light(my_id, f, e, Type::Point)
{
    f->read(position_max_radius.x);
    f->read(position_max_radius.y);
    f->read(position_max_radius.z);
    update_influence_area();
}

const gearoenix::math::Vec4<double>& gearoenix::render::light::Point::get_position_max_radius() const noexcept
{
    return position_max_radius;
}

double gearoenix::render::light::Point::get_min_radius() const noexcept
{
    return min_radius;
}

bool gearoenix::render::light::Point::is_in_light(const model::Model* const) const noexcept
{
    GXUNIMPLEMENTED // check object visibility
}

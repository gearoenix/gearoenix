#include "rnd-mdl-uniform.hpp"

gearoenix::render::model::Uniform::Uniform() {}

gearoenix::math::Mat4x4& gearoenix::render::model::Uniform::get_m()
{
    return m;
}

const gearoenix::math::Mat4x4& gearoenix::render::model::Uniform::get_m() const
{
    return m;
}

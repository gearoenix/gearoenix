#include "rnd-mdl-uniform.hpp"

gearoenix::render::model::Uniform::Uniform() {}

inline gearoenix::math::Mat4x4& gearoenix::render::model::Uniform::get_m()
{
    return m;
}

inline const gearoenix::math::Mat4x4& gearoenix::render::model::Uniform::get_m() const
{
    return m;
}

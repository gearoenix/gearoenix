#include "rnd-mdl-transformation.hpp"
#include "../../math/math-sphere.hpp"
#include "rnd-mdl-uniform.hpp"

gearoenix::render::model::Transformation::Transformation(Uniform* const uniform, math::Sphere* const sphere) noexcept
    : uniform(uniform)
    , sphere(sphere)
{
}

void gearoenix::render::model::Transformation::set_location(const math::Vec3& l) noexcept
{
    uniform->m.set_location(l);
    sphere->position = l;
}

void gearoenix::render::model::Transformation::local_scale(const core::Real s) noexcept
{
    uniform->m.local_scale(s);
    sphere->radius *= s;
}

void gearoenix::render::model::Transformation::local_x_scale(const core::Real s) noexcept
{
    uniform->m.local_scale(s, 1.0f, 1.0f);
    sphere->radius *= s;
}

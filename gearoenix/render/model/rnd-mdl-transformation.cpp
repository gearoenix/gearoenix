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
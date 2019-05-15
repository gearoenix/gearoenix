#include "rnd-mdl-transformation.hpp"
#include "rnd-mdl-uniform.hpp"

gearoenix::render::model::Transformation::Transformation(Uniform* const uniform) noexcept
	: uniform(uniform)
{
}

gearoenix::render::model::Transformation::~Transformation() noexcept {}

void gearoenix::render::model::Transformation::set_location(const math::Vec3& l) noexcept 
{
	uniform->m.set_location(l);
}
#include "rnd-lt-directional.hpp"

const gearoenix::math::Mat4x4 & gearoenix::render::light::Directional::get_view_projection_biases() const
{
	return view_projection_biases;
}

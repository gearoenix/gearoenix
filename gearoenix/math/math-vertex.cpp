#include "math-vertex.hpp"

gearoenix::math::BasicVertex::BasicVertex(const std::shared_ptr<system::stream::Stream>& f)
{
	read(f);
}

void gearoenix::math::BasicVertex::read(const std::shared_ptr<system::stream::Stream>& f)
{
	position.read(f);
	normal.read(f);
	tangent.read(f);
	uv.read(f);
}

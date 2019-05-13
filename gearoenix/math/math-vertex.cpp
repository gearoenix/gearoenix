#include "math-vertex.hpp"

gearoenix::math::BasicVertex::BasicVertex()
{
}

gearoenix::math::BasicVertex::BasicVertex(const Vec3& position, const Vec3& normal, const Vec4& tangent, const Vec2& uv)
	: position(position)
	, normal(normal)
	, tangent(tangent)
	, uv(uv)
{
}

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

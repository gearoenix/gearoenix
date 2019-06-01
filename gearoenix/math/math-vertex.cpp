#include "math-vertex.hpp"

gearoenix::math::BasicVertex::BasicVertex(const Vec3& position, const Vec3& normal, const Vec4& tangent, const Vec2& uv) noexcept
    : position(position)
    , normal(normal)
    , tangent(tangent)
    , uv(uv)
{
}

gearoenix::math::BasicVertex::BasicVertex(system::stream::Stream* const f) noexcept
{
    read(f);
}

void gearoenix::math::BasicVertex::read(system::stream::Stream* const f) noexcept
{
    position.read(f);
    normal.read(f);
    tangent.read(f);
    uv.read(f);
}

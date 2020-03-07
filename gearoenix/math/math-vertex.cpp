#include "math-vertex.hpp"

gearoenix::math::BasicVertex::BasicVertex(const Vec3<float>& position, const Vec3<float>& normal, const Vec4<float>& tangent, const Vec2<float>& uv) noexcept
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

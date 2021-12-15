#include "gx-rnd-vertex.hpp"

gearoenix::render::PbrVertex::PbrVertex(
    const math::Vec3<float>& position,
    const math::Vec3<float>& normal,
    const math::Vec4<float>& tangent,
    const math::Vec2<float>& uv) noexcept
    : position(position)
    , normal(normal)
    , tangent(tangent)
    , uv(uv)
{
}

gearoenix::render::PbrVertex::PbrVertex(platform::stream::Stream& f) noexcept
{
    read(f);
}

void gearoenix::render::PbrVertex::read(platform::stream::Stream& f) noexcept
{
    position.read(f);
    normal.read(f);
    tangent.read(f);
    uv.read(f);
}

void gearoenix::render::PbrVertex::set_position(const float x, const float y, const float z) noexcept
{
    position.x = x;
    position.y = y;
    position.z = z;
}

void gearoenix::render::PbrVertex::set_uv(const float x, const float y) noexcept
{
    uv.x = x;
    uv.y = y;
}

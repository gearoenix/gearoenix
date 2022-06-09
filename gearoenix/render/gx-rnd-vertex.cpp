#include "gx-rnd-vertex.hpp"

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
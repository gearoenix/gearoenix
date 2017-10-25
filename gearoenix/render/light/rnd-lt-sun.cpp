#include "rnd-lt-sun.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"

gearoenix::render::light::Sun::Sun(system::File* f)
    : Light(f)
    , cam(new camera::Orthographic(f, nullptr))
{
    color.read(f);
}

gearoenix::render::light::Sun::~Sun()
{
    delete cam;
}

const gearoenix::math::Vec3& gearoenix::render::light::Sun::get_direction() const
{
    return cam->get_z();
}

const gearoenix::math::Vec3& gearoenix::render::light::Sun::get_color() const
{
    return color;
}

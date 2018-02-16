#include "rnd-lt-sun.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"

gearoenix::render::light::Sun::Sun(system::stream::Stream* f)
    : Light(f)
    , cam(new camera::Orthographic(f, nullptr))
{
    color.read(f);
    db = math::Mat4x4(
#ifdef IN_WINDOWS
             0.5, 0.0, 0.0, 0.0,
             0.0, -0.5, 0.0, 0.0,
             0.0, 0.0, 1.0, 0.0,
             0.5, 0.5, 0.0, 1.0)
#else
             0.5, 0.0, 0.0, 0.0,
             0.0, 0.5, 0.0, 0.0,
             0.0, 0.0, 0.5, 0.0,
             0.5, 0.5, 0.5, 1.0)
#endif
        * cam->get_view_projection();
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

bool gearoenix::render::light::Sun::get_color_changed() const
{
    return color_changed;
}

const gearoenix::math::Mat4x4& gearoenix::render::light::Sun::get_bias() const
{
    return db;
}

const gearoenix::render::camera::Orthographic* gearoenix::render::light::Sun::get_camera() const
{
    return cam;
}

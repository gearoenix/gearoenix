#include "rnd-lt-sun.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"

gearoenix::render::light::Sun::Sun(system::File* f)
    : Light(f)
    , cam(new camera::Orthographic(f, nullptr))
{
}

gearoenix::render::light::Sun::~Sun()
{
    delete cam;
}

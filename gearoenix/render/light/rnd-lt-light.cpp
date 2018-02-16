#include "rnd-lt-light.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "rnd-lt-sun.hpp"

gearoenix::render::light::Light::Light(system::stream::Stream*) {}

gearoenix::render::light::Light::~Light() {}

gearoenix::render::light::Light* gearoenix::render::light::Light::read(system::stream::Stream* f)
{
    core::Id light_type;
    f->read(light_type);
    switch (light_type) {
    case 1:
        return new Sun(f);
    default:
        UNEXPECTED;
    }
    return nullptr;
}

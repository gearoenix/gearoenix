#include "rnd-lt-light.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "rnd-lt-sun.hpp"

gearoenix::render::light::Light::Light(core::Id my_id, system::stream::Stream*, Engine*)
    : core::asset::Asset(my_id, core::asset::Type::LIGHT)
{
}

gearoenix::render::light::Light::~Light() {}

gearoenix::render::light::Light* gearoenix::render::light::Light::read(core::Id my_id, system::stream::Stream* f, Engine* e)
{
    core::Id light_type;
    f->read(light_type);
    switch (light_type) {
    case 1:
        return new Sun(my_id, f, e);
    default:
        GXUNEXPECTED;
    }
    return nullptr;
}

#include "rnd-lt-light.hpp"
#include "../../system/file/sys-fl-file.hpp"
#include "rnd-lt-sun.hpp"

gearoenix::render::light::Light::Light(system::file::File*) {}

gearoenix::render::light::Light::~Light() {}

gearoenix::render::light::Light* gearoenix::render::light::Light::read(system::file::File* f)
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

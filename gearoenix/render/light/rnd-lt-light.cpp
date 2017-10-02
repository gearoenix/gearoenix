#include "rnd-lt-light.hpp"
#include "../../system/sys-file.hpp"
#include "rnd-lt-sun.hpp"

gearoenix::render::light::Light::Light(system::File*) {}

gearoenix::render::light::Light::~Light() {}

gearoenix::render::light::Light* gearoenix::render::light::Light::read(system::File* f)
{
    core::Id light_type;
    f->read(light_type);
    switch (light_type) {
    case 10:
        return new Sun(f);
    }
    LOGF("Unexpected");
}

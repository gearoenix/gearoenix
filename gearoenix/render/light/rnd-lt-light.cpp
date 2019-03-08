#include "rnd-lt-light.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-lt-sun.hpp"

gearoenix::render::light::Light::Light(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e)
    : core::asset::Asset(my_id, core::asset::Type::LIGHT)
{
}

gearoenix::render::light::Light::~Light() {}

gearoenix::render::light::Light* gearoenix::render::light::Light::read(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e)
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

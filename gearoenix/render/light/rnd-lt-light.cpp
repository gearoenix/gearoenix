#include "rnd-lt-light.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../engine/rnd-eng-engine.hpp"

gearoenix::render::light::Light::Light(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const Type light_type) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Light)
    , light_type(light_type)
    , e(e)
{
    color.read(f);
    has_shadow = f->read_bool();
}

gearoenix::render::light::Light::Light(const core::Id my_id, engine::Engine* const e, const Type light_type) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Light)
    , light_type(light_type)
    , e(e)
{
}

bool gearoenix::render::light::Light::is_shadow_caster() const noexcept
{
    return has_shadow;
}

void gearoenix::render::light::Light::enable_shadowing() noexcept
{
    has_shadow = true;
}

void gearoenix::render::light::Light::disable_shadowing() noexcept
{
    has_shadow = false;
}

bool gearoenix::render::light::Light::is_enabled() const noexcept
{
    return enabled;
}

void gearoenix::render::light::Light::enable() noexcept
{
    enabled = true;
}

void gearoenix::render::light::Light::disable() noexcept
{
    enabled = false;
}
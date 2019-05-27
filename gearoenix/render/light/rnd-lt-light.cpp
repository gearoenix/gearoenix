#include "rnd-lt-light.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/rnd-eng-engine.hpp"

static std::shared_ptr<gearoenix::render::buffer::FramedUniform> null_uniforms;

gearoenix::render::light::Light::Light(
    const core::Id my_id,
    system::stream::Stream*const f,
    engine::Engine*const e) noexcept
    : core::asset::Asset(my_id, core::asset::Type::LIGHT)
    , e(e)
{
    f->read(color[0]);
    f->read(color[1]);
    f->read(color[2]);
    has_shadow = f->read_bool();
}

gearoenix::render::light::Light::Light(const core::Id my_id, engine::Engine*const e) noexcept
    : core::asset::Asset(my_id, core::asset::Type::LIGHT)
    , e(e)
{
}

const gearoenix::math::Vec3& gearoenix::render::light::Light::get_color() const noexcept
{
    return color;
}

void gearoenix::render::light::Light::set_color(const gearoenix::math::Vec3& c) noexcept
{
    color = c;
}

bool gearoenix::render::light::Light::is_shadower() const noexcept
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

const gearoenix::math::Aabb3& gearoenix::render::light::Light::get_influence_area() const noexcept
{
    return influence;
}

void gearoenix::render::light::Light::set_influence_area(const gearoenix::math::Aabb3& a) noexcept
{
    influence = a;
}

void gearoenix::render::light::Light::update_uniform() noexcept
{
}

const std::shared_ptr<gearoenix::render::buffer::FramedUniform>& gearoenix::render::light::Light::get_uniform_buffers() const noexcept
{
    return null_uniforms;
}

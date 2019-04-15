#include "rnd-lt-light.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/rnd-eng-engine.hpp"

static std::shared_ptr<gearoenix::render::buffer::FramedUniform> null_uniforms;

gearoenix::render::light::Light::Light(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<engine::Engine>& e)
    : core::asset::Asset(my_id, core::asset::Type::LIGHT)
{
    color = math::Vec4(f->read<core::Real>(), f->read<core::Real>(), f->read<core::Real>(), 1.0f);
    has_shadow = f->read_bool();
}

gearoenix::render::light::Light::~Light() {}

const gearoenix::math::Vec4& gearoenix::render::light::Light::get_color() const
{
    return color;
}

bool gearoenix::render::light::Light::is_shadower() const
{
    return has_shadow;
}

void gearoenix::render::light::Light::enable_shadowing()
{
    has_shadow = true;
}

void gearoenix::render::light::Light::disable_shadowing()
{
    has_shadow = false;
}

bool gearoenix::render::light::Light::is_enabled() const
{
    return enabled;
}

void gearoenix::render::light::Light::enable()
{
    enabled = true;
}

void gearoenix::render::light::Light::disable()
{
    enabled = false;
}

void gearoenix::render::light::Light::update_uniform()
{
}

const std::shared_ptr<gearoenix::render::buffer::FramedUniform>& gearoenix::render::light::Light::get_uniform_buffers() const
{
    return null_uniforms;
}

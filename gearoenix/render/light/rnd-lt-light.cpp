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
	color_strength.read(f);
	has_shadow = f->read_bool();
}

gearoenix::render::light::Light::~Light() {}

const gearoenix::math::Vec3 & gearoenix::render::light::Light::get_color() const
{
	return *reinterpret_cast<const math::Vec3 *>(&color_strength);
}

const gearoenix::math::Vec4 & gearoenix::render::light::Light::get_color_strength() const
{
	return color_strength;
}

const gearoenix::core::Real gearoenix::render::light::Light::strength() const
{
	return color_strength[3];
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

const std::shared_ptr<gearoenix::render::buffer::FramedUniform>& gearoenix::render::light::Light::get_uniform_buffers() const
{
	return nullptr;
}

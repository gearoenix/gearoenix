#include "rnd-lt-directional.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"

gearoenix::render::light::Directional::Directional(const core::Id my_id, system::stream::Stream* const f, engine::Engine* const e) noexcept
    : Light(my_id, f, e, Type::DIRECTIONAL)
{
    direction.read(f);
}

gearoenix::render::light::Directional::Directional(const core::Id my_id, engine::Engine* const e) noexcept
    : Light(my_id, e, Type::DIRECTIONAL)
{
}

const gearoenix::math::Vec3& gearoenix::render::light::Directional::get_direction() const noexcept
{
    return direction;
}

void gearoenix::render::light::Directional::set_direction(const gearoenix::math::Vec3& dir) noexcept
{
    direction = dir;
}

bool gearoenix::render::light::Directional::is_in_light(const model::Model*) const noexcept
{
    // Directional light is a universal light and always try to import frustum related objects
    return true;
}
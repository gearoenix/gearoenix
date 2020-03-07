#include "rnd-lt-directional.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../scene/rnd-scn-scene.hpp"

gearoenix::render::light::Directional::Directional(const core::Id my_id, system::stream::Stream* const f, engine::Engine* const e) noexcept
    : Light(my_id, f, e, Type::Directional)
{
    direction.read(f);
}

gearoenix::render::light::Directional::Directional(const core::Id my_id, engine::Engine* const e) noexcept
    : Light(my_id, e, Type::Directional)
    , direction(0.0f, 0.0f, -1.0f)
{
}

gearoenix::render::light::Directional::~Directional() noexcept = default;

bool gearoenix::render::light::Directional::is_in_light(const model::Model*) const noexcept
{
    // Directional light is a universal light and always try to import frustum related objects
    return true;
}

#include "rnd-rfl-reflection.hpp"

gearoenix::render::reflection::Reflection::Reflection(
    const core::Id id,
    const Type t,
    render::engine::Engine* const e) noexcept
    : Asset(id, core::asset::Type::Reflection)
    , reflection_type(t)
    , e(e)
{
}

gearoenix::render::reflection::Reflection::~Reflection() noexcept = default;

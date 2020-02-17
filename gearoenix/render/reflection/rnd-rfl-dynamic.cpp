#include "rnd-rfl-dynamic.hpp"

gearoenix::render::reflection::Dynamic::Dynamic(const core::Id id, engine::Engine* const e) noexcept
    : Reflection(id, Type::Dynamic, e)
{
}

gearoenix::render::reflection::Dynamic::~Dynamic() noexcept = default;

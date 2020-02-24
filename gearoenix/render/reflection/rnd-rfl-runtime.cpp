#include "rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Runtime::Runtime(const core::Id id, engine::Engine* const e) noexcept
    : Reflection(id, Type::Runtime, e)
{
}

gearoenix::render::reflection::Runtime::~Runtime() noexcept = default;

#include "rnd-mdl-dynamic.hpp"
#include "rnd-mdl-transformation.hpp"

gearoenix::render::model::Dynamic::Dynamic(
    const core::Id my_id,
    std::string name,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Model(my_id, std::move(name), Type::Dynamic, f, e, c)
{
}

gearoenix::render::model::Dynamic::Dynamic(
    const core::Id my_id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Model(my_id, std::move(name), Type::Dynamic, e, c)
{
}

gearoenix::render::model::Dynamic::Dynamic(const Dynamic& o) noexcept = default;

gearoenix::render::model::Model* gearoenix::render::model::Dynamic::clone() const noexcept
{
    return new Dynamic(*this);
}
#include "rnd-mdl-static.hpp"
#include "rnd-mdl-transformation.hpp"

gearoenix::render::model::Static::Static(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Model(my_id, Type::Static, f, e, c)
{
}

gearoenix::render::model::Static::Static(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Model(my_id, Type::Static, e, c)
{
}

void gearoenix::render::model::Static::set_scene(scene::Scene* const s) noexcept
{
    Model::set_scene(s);
    transformation = nullptr;
}

bool gearoenix::render::model::Static::get_dynamicity() const noexcept
{
    return false;
}

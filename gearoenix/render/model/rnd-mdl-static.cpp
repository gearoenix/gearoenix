#include "rnd-mdl-static.hpp"
#include "rnd-mdl-transformation.hpp"

gearoenix::render::model::Static::Static(
    const core::Id my_id,
    std::string name,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Model(my_id, std::move(name), Type::Static, f, e, c)
{
}

gearoenix::render::model::Static::Static(
    const core::Id my_id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Model(my_id, std::move(name), Type::Static, e, c)
{
}

#define GX_STATIC_CONS(...)                                \
    std::shared_ptr<Static> self(new Static(__VA_ARGS__)); \
    self->model_self = self;                               \
    self->static_model_self = self;                        \
    return self

std::shared_ptr<gearoenix::render::model::Static> gearoenix::render::model::Static::construct(
    const core::Id id,
    std::string name,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    GX_STATIC_CONS(id, std::move(name), f, e, c);
}

std::shared_ptr<gearoenix::render::model::Static> gearoenix::render::model::Static::construct(
    const core::Id id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    GX_STATIC_CONS(id, std::move(name), e, c);
}

gearoenix::render::model::Static::~Static() noexcept = default;

void gearoenix::render::model::Static::set_scene(scene::Scene* const s) noexcept
{
    Model::set_scene(s);
    transformation = nullptr;
}

bool gearoenix::render::model::Static::get_dynamicity() const noexcept
{
    return false;
}

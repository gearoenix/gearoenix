#include "gx-rnd-mdl-dynamic.hpp"
#include "gx-rnd-mdl-transformation.hpp"

gearoenix::render::model::Dynamic::Dynamic(
    const core::Id my_id,
    std::string name,
    platform::stream::Stream* const f,
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

#define GX_DYN_CONS(...)                                     \
    std::shared_ptr<Dynamic> self(new Dynamic(__VA_ARGS__)); \
    self->model_self = self;                                 \
    self->dynamic_model_self = self;                         \
    return self

std::shared_ptr<gearoenix::render::model::Dynamic> gearoenix::render::model::Dynamic::construct(
    const core::Id id,
    std::string name,
    platform::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    GX_DYN_CONS(id, std::move(name), f, e, c);
}

std::shared_ptr<gearoenix::render::model::Dynamic> gearoenix::render::model::Dynamic::construct(
    const core::Id id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    GX_DYN_CONS(id, std::move(name), e, c);
}

gearoenix::render::model::Dynamic::~Dynamic() noexcept = default;

std::shared_ptr<gearoenix::render::model::Model> gearoenix::render::model::Dynamic::clone() const noexcept
{
    GX_DYN_CONS(*this);
}

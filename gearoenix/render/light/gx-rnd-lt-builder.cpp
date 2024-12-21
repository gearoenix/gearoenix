#include "gx-rnd-lt-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-allocator.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../light/gx-rnd-lt-light.hpp"

gearoenix::render::light::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    physics::Transformation* const parent_transform,
    const DirectionalInfo&,
    core::job::EndCaller<>&& end_callback)
    : e(e)
    , entity_builder(std::make_shared<core::ecs::EntitySharedBuilder>(std::string(name), std::move(end_callback)))
{
    auto& b = entity_builder->get_builder();
    b.add_component(core::ecs::construct_component<Directional>(name + "-directional-light", b.get_id()));
    b.add_component(core::ecs::construct_component<physics::Transformation>(name + "-light-directional-transform", parent_transform, b.get_id()));
}

gearoenix::render::light::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    physics::Transformation* const parent_transform,
    const ShadowCasterDirectionalInfo&,
    core::job::EndCaller<>&& end_callback)
    : e(e)
    , entity_builder(std::make_shared<core::ecs::EntitySharedBuilder>(std::string(name), std::move(end_callback)))
{
    auto& b = entity_builder->get_builder();
    b.add_component(core::ecs::construct_component<physics::Transformation>(
        name + "-light-directional-shadow-caster-transform", parent_transform, b.get_id()));
}

gearoenix::render::light::Builder::~Builder() = default;

const gearoenix::render::light::ShadowCasterDirectional* gearoenix::render::light::Builder::get_shadow_caster_directional() const
{
    return entity_builder->get_builder().get_component<ShadowCasterDirectional>();
}

gearoenix::render::light::ShadowCasterDirectional* gearoenix::render::light::Builder::get_shadow_caster_directional()
{
    return entity_builder->get_builder().get_component<ShadowCasterDirectional>();
}

const gearoenix::render::light::Light& gearoenix::render::light::Builder::get_light() const
{
    return *entity_builder->get_builder().get_component<Light>();
}

gearoenix::render::light::Light& gearoenix::render::light::Builder::get_light()
{
    return *entity_builder->get_builder().get_component<Light>();
}

gearoenix::core::ecs::entity_id_t gearoenix::render::light::Builder::get_id() const
{
    return entity_builder->get_id();
}

#include "gx-rnd-lt-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../light/gx-rnd-lt-light.hpp"

gearoenix::render::light::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    const DirectionalInfo&,
    core::job::EndCaller<>&& end_callback)
    : e(e)
    , entity_builder(e.get_world()->create_shared_builder(std::string(name), std::move(end_callback)))
{
    auto& b = entity_builder->get_builder();
    b.add_component(Directional::construct(name + "-directional-light"));
}

gearoenix::render::light::Builder::Builder(
    engine::Engine& e,
    const std::string& name,
    const ShadowCasterDirectionalInfo&,
    core::job::EndCaller<>&& end_callback)
    : e(e)
    , entity_builder(e.get_world()->create_shared_builder(std::string(name), std::move(end_callback)))
{
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

const gearoenix::physics::Transformation* gearoenix::render::light::Builder::get_transformation() const
{
    return entity_builder->get_builder().get_component<physics::Transformation>();
}

gearoenix::physics::Transformation* gearoenix::render::light::Builder::get_transformation()
{
    return entity_builder->get_builder().get_component<physics::Transformation>();
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

#include "gx-rnd-lt-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "gx-rnd-lt-directional.hpp"
#include "gx-rnd-lt-point.hpp"

gearoenix::render::light::Manager::Manager()
    : Singleton(this)
{
    core::ecs::ComponentType::add<Light>();
    core::ecs::ComponentType::add<Directional>();
    core::ecs::ComponentType::add<Point>();
    core::ecs::ComponentType::add<ShadowCasterDirectional>();
}

gearoenix::render::light::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::render::light::Manager::build_directional(std::string&& name, core::ecs::Entity* const parent)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<Directional>(name + "-directional-light"));
    entity->add_component(core::Object::construct<physics::Transformation>(name + "-light-directional-transform"));
    return entity;
}

void gearoenix::render::light::Manager::build_shadow_caster_directional(
    std::string&& name,
    core::ecs::Entity* const parent,
    const std::uint32_t,
    const float,
    const float,
    const float,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<physics::Transformation>(entity->get_object_name() + "-light-directional-shadow-caster-transform"));
    entity_callback.set_return(std::move(entity));
}

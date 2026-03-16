#include "gx-phs-cns-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "gx-phs-cns-constraint.hpp"
#include "gx-phs-cns-jet-controller.hpp"

gearoenix::physics::constraint::Manager::Manager()
    : Singleton(this)
{
    core::ecs::ComponentType::add<Constraint>();
    core::ecs::ComponentType::add<JetController>();
}

gearoenix::physics::constraint::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::physics::constraint::Manager::create_jet_controller(std::string&& name, std::shared_ptr<Transformation>&& transform, core::ecs::Entity* const parent) const
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<JetController>(entity.get(), std::move(transform), entity->get_object_name() + "-jet-controller"));
    return entity;
}

void gearoenix::physics::constraint::Manager::update()
{
    core::ecs::World::get().parallel_system<Constraint>([&](const auto, auto* const cns, const auto) { cns->update(); });
}

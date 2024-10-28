#include "gx-phs-cns-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-phs-cns-constraint.hpp"
#include "gx-phs-cns-jet-controller.hpp"

gearoenix::physics::constraint::Manager::Manager(render::engine::Engine& e)
    : e(e)
{
}

gearoenix::physics::constraint::Manager::~Manager() = default;

std::shared_ptr<gearoenix::core::ecs::EntitySharedBuilder> gearoenix::physics::constraint::Manager::create_jet_controller(
    std::string&& name,
    std::shared_ptr<Transformation>&& transform,
    core::job::EndCaller<>&& entity_exists_in_world) const
{
    auto cmp_name = name + "-jet-controller";
    auto eb = e.get_world()->create_shared_builder(std::move(name), std::move(entity_exists_in_world));
    eb->get_builder().add_component(JetController::construct(e, std::move(transform), std::move(cmp_name), eb->get_id()));
    return eb;
}

void gearoenix::physics::constraint::Manager::update()
{
    e.get_world()->parallel_system<Constraint>([&](const auto, auto* const cns, const auto) {
        cns->update();
    });
}

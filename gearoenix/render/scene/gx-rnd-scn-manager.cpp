#include "gx-rnd-scn-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-scn-scene.hpp"

gearoenix::render::scene::Manager::Manager()
    : Singleton(this)
{
    core::ecs::ComponentType::add<Scene>();
}

gearoenix::render::scene::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::render::scene::Manager::build(std::string&& name, const double) const
{
    return core::ecs::Entity::construct(std::move(name), nullptr);
}

void gearoenix::render::scene::Manager::update()
{
    core::ecs::World::get().parallel_system<Scene>([](auto* const, Scene* const s, const auto /*kernel_index*/) {
        s->update();
    });
}

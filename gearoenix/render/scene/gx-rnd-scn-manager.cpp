#include "gx-rnd-scn-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-scn-scene.hpp"

namespace {
gearoenix::render::scene::Manager* instance = nullptr;
}

gearoenix::render::scene::Manager::Manager()
{
    core::ecs::ComponentType::add<Scene>();
    GX_ASSERT_D(!instance);
    instance = this;
}

gearoenix::render::scene::Manager::~Manager()
{
    GX_ASSERT_D(instance);
    instance = nullptr;
}

gearoenix::render::scene::Manager& gearoenix::render::scene::Manager::get()
{
    GX_ASSERT_D(instance);
    return *instance;
}

gearoenix::core::ecs::EntityPtr gearoenix::render::scene::Manager::build(std::string&& name, const double) const
{
    return core::ecs::Entity::construct(std::move(name), nullptr);
}

void gearoenix::render::scene::Manager::update() const
{
    core::ecs::World::get().parallel_system<Scene>([](auto* const, auto* const s, const auto /*kernel_index*/) {
        s->update();
    });
}

#include "gx-rnd-scn-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/gx-cr-profiler.hpp"
#include "gx-rnd-scn-scene.hpp"

gearoenix::render::scene::Manager::Manager()
    : Singleton(this)
{
    core::ecs::ComponentType::add<Scene>();
}

gearoenix::render::scene::Manager::~Manager() = default;

void gearoenix::render::scene::Manager::update()
{
    core::ecs::World::get().parallel_system<Scene>([](auto* const, Scene* const s, const auto /*kernel_index*/) {
        GX_PROFILE_EXP(s->update_per_frame());
    });
}

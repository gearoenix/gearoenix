#include "gx-rnd-rcd-scene.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"

void gearoenix::render::record::Scene::update(core::ecs::Entity* const scene_entity)
{
    constexpr std::array par_fns = {
        std::function([&] { cameras.update(scene_entity); }),
        std::function([&] { lights.update(scene_entity); }),
        std::function([&] { models.update(scene_entity); }),
        std::function([&] { reflections.update(scene_entity); }),
        std::function([&] { skyboxes.update(scene_entity); }),
    };

    core::sync::ParallelFor::exec(par_fns.begin(), par_fns.end(), [&](auto& f) { f(); });
}
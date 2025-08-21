#include "gx-rnd-rcd-scene.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"

void gearoenix::render::record::Scene::update(core::ecs::Entity* const scene_entity)
{
    const std::array par_fns = {
        std::function<void()>([&] -> void { cameras.update(scene_entity); }),
        std::function<void()>([&] -> void { lights.update(scene_entity); }),
        std::function<void()>([&] -> void { models.update(scene_entity); }),
        std::function<void()>([&] -> void { reflections.update(scene_entity); }),
        std::function<void()>([&] -> void { skyboxes.update(scene_entity); }),
    };

    core::sync::parallel_for(par_fns, [&](const auto& f, const auto) { f(); });

    cameras.update_models(models);
    lights.update_dynamic_models(models);
    lights.update_static_models(models);
    reflections.update_dynamic_models(models);
    reflections.update_static_models(models);
}
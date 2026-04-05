#include "gx-rnd-rcd-scene.hpp"
#include "../../core/gx-cr-profiler.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"

gearoenix::render::record::Scene::Scene()
    : parallel_functions_1 {
        std::function([this](core::ecs::Entity* const scene_entity) -> void {
            GX_PROFILE_EXP(cameras.update(scene_entity));
        }),
        std::function([this](core::ecs::Entity* const scene_entity) -> void {
            GX_PROFILE_EXP(lights.update_per_frame(scene_entity));
        }),
        std::function([this](core::ecs::Entity* const scene_entity) -> void {
            GX_PROFILE_EXP(models.update_per_frame(scene_entity));
        }),
        std::function([this](core::ecs::Entity* const scene_entity) -> void {
            GX_PROFILE_EXP(reflections.update(scene_entity));
        }),
        std::function([this](core::ecs::Entity* const scene_entity) -> void {
            GX_PROFILE_EXP(skyboxes.update(scene_entity));
        }),
    }
{
}

gearoenix::render::record::Scene::~Scene() = default;

void gearoenix::render::record::Scene::update_per_frame(core::ecs::Entity* const scene_entity)
{
    {
        GX_PROFILE_SCOPE(render_record_Scene_update_first_part);
        core::sync::parallel_for(parallel_functions_1, [&](const auto& f, const auto) {
            f(scene_entity);
        });
    }

    GX_PROFILE_EXP(cameras.update_models(models));

    GX_PROFILE_EXP(lights.update_dynamic_models(models));
    GX_PROFILE_EXP(lights.update_static_models(models));

    GX_PROFILE_EXP(reflections.update_dynamic_models(models));
    GX_PROFILE_EXP(reflections.update_static_models(models));
}

void gearoenix::render::record::Scene::update_after_change(core::ecs::Entity* const scene_entity)
{
    const std::array fun_arr {
        std::function([this](core::ecs::Entity* const se) {
            GX_PROFILE_EXP(models.update_after_change(se));
        }),
        std::function([this](core::ecs::Entity* const se) {
            GX_PROFILE_EXP(lights.update_after_change(se));
        }),
    };

    GX_PROFILE_EXP(core::sync::parallel_for(fun_arr, [&](const auto& f, auto) { f(scene_entity); }));
}
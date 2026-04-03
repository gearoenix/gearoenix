#include "gx-rnd-rcd-scene.hpp"
#include "../../core/gx-cr-profiler.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"

void gearoenix::render::record::Scene::update(core::ecs::Entity* const scene_entity)
{
    const std::array par_fns = {
        std::function<void()>([&] -> void {
            GX_PROFILE_SCOPE(render_record_Scene_update_cameras.update);
            cameras.update(scene_entity);
        }),
        std::function<void()>([&] -> void {
            GX_PROFILE_SCOPE(render_record_Scene_update_lights.update);
            lights.update(scene_entity);
        }),
        std::function<void()>([&] -> void {
            GX_PROFILE_SCOPE(render_record_Scene_update_models);
            models.update(scene_entity);
        }),
        std::function<void()>([&] -> void {
            GX_PROFILE_SCOPE(render_record_Scene_update_reflections.update);
            reflections.update(scene_entity);
        }),
        std::function<void()>([&] -> void {
            GX_PROFILE_SCOPE(render_record_Scene_update_skyboxes);
            skyboxes.update(scene_entity);
        }),
    };

    {
        GX_PROFILE_SCOPE(render_record_Scene_update_first_part);
        core::sync::parallel_for(par_fns, [&](const auto& f, const auto) {
            f();
        });
    }

    {
        GX_PROFILE_SCOPE(render_record_Scene_update_cameras.update_models);
        cameras.update_models(models);
    }

    {
        GX_PROFILE_SCOPE(render_record_Scene_update_lights.update_dynamic_models);
        lights.update_dynamic_models(models);
    }

    {
        GX_PROFILE_SCOPE(render_record_Scene_update_third_part);
        lights.update_static_models(models);
    }

    {
        GX_PROFILE_SCOPE(render_record_Scene_update_reflections.update_dynamic_models);
        reflections.update_dynamic_models(models);
    }

    {
        GX_PROFILE_SCOPE(render_record_Scene_update_reflections.update_static_models);
        reflections.update_static_models(models);
    }
}
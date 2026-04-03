#include "gx-rnd-rcd-light.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "gx-rnd-rcd-model.hpp"

void gearoenix::render::record::Lights::update_per_frame(core::ecs::Entity*const)
{
}

void gearoenix::render::record::Lights::update_after_change(core::ecs::Entity* const scene_entity)
{
    struct ThreadData final {
        std::vector<ShadowCasterDirectionalLightData> shadow_caster_directionals;
    };

    std::vector<ThreadData> threads(core::sync::threads_count());
    shadow_caster_directionals.clear();

    core::ecs::World::get().parallel_system<core::ecs::All<light::ShadowCasterDirectional, physics::Transformation>>([&](auto* const e, auto* const l, auto* const trn, const auto kernel_index) {
        if (!l->get_enabled() || !trn->get_enabled() || !e->contains_in_parents(scene_entity)) {
            return;
        }

        threads[kernel_index].shadow_caster_directionals.push_back(ShadowCasterDirectionalLightData {
            .frustum = &l->get_shadow_frustum()->get_frustum(),
            .normalised_vp = &l->get_shadow_camera()->get_view_projection(),
            .direction = &l->get_shadow_transform()->get_z_axis(),
            .colour = &l->colour,
            .entity = e,
            .shadow_caster_directional = l,
            .transform = trn,
        });
    });

    for (const auto& t : threads) {
        for (const auto& l : t.shadow_caster_directionals) {
            shadow_caster_directionals.push_back(l);
        }
    }
}

void gearoenix::render::record::Lights::update_models(physics::accelerator::Bvh& bvh, std::vector<Model>& models)
{
    bvh.call_on_all([&](auto& n) -> void {
        auto& l = models[n.index].lights;
        l.shadow_caster_directionals.clear();
        l.directionals.clear();
    });

    for (auto& shadow : shadow_caster_directionals) {
        bvh.call_on_intersecting(
            *shadow.frustum,
            [&](auto& node) {
                auto& l = models[node.index].lights;
                if (l.shadow_caster_directionals.size() == l.shadow_caster_directionals.max_size()) {
                    if (l.directionals.size() == l.directionals.max_size()) {
                        return;
                    }
                    l.directionals.push_back(ModelDirectionalLight {
                        .direction = shadow.direction->to<float>(),
                        .light = shadow.shadow_caster_directional });
                    return;
                }
                l.shadow_caster_directionals.push_back(&shadow);
            },
            [&](auto& node) {
                auto& l = models[node.index].lights;
                if (l.directionals.size() == l.directionals.max_size()) {
                    return;
                }
                l.directionals.push_back(ModelDirectionalLight {
                    .direction = shadow.direction->to<float>(),
                    .light = shadow.shadow_caster_directional,
                });
            });
    }
}

void gearoenix::render::record::Lights::update_dynamic_models(Models& models)
{
    update_models(models.dynamic_models_bvh, models.models);
}

void gearoenix::render::record::Lights::update_static_models(Models& models)
{
    /// TODO: later we can have static and dynamic (in terms of transformation) lights and we can cache the following computation.
    update_models(models.static_models_bvh, models.models);
}
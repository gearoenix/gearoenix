#include "gx-rnd-rcd-light.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../model/gx-rnd-mdl-uniform.hpp"
#include "gx-rnd-rcd-model.hpp"

namespace {
template <typename Shadow, typename Model>
void on_intersection(Shadow& shadow, Model& model)
{
    auto& u = model.model->get_uniform_inner_pointer();
    auto& counts = u.bones_point_lights_directional_lights_shadow_caster_directional_lights_count;
    if (auto& count = counts.w; count < GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER) {
        u.shadow_caster_directional_lights[count >> 2][count & 3] = shadow.shadow_caster_directional->get_uniform().shader_data_index;
        ++count;
        return;
    }
    if (auto& count = counts.z; count < GX_RENDER_MAX_DIRECTIONAL_LIGHTS) {
        u.directional_lights[count >> 2][count & 3] = shadow.shadow_caster_directional->get_directional_uniform().shader_data_index;
        ++count;
    }
}

template <typename Shadow, typename Model>
void not_intersection(Shadow& shadow, Model& model)
{
    auto& u = model.model->get_uniform_inner_pointer();
    auto& counts = u.bones_point_lights_directional_lights_shadow_caster_directional_lights_count;
    if (auto& count = counts.z; count < GX_RENDER_MAX_DIRECTIONAL_LIGHTS) {
        u.directional_lights[count >> 2][count & 3] = shadow.shadow_caster_directional->get_directional_uniform().shader_data_index;
        ++count;
    }
}

template <typename Shadow, typename Model>
void check_intersection(Shadow& shadow, Model& model)
{
    if (shadow.frustum->check_intersection(model.collider->get_surrounding_box())) {
        on_intersection(shadow, model);
    } else {
        not_intersection(shadow, model);
    }
}

template <typename Model>
void clear_lights(Model& model)
{
    auto& count = model.model->get_uniform_inner_pointer().bones_point_lights_directional_lights_shadow_caster_directional_lights_count;
    count.y = 0;
    count.z = 0;
    count.w = 0;
}
}

void gearoenix::render::record::Lights::update_per_frame(core::ecs::Entity* const)
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
        clear_lights(models[n.index]);
    });

    for (auto& shadow : shadow_caster_directionals) {
        bvh.call_on_intersecting(
            *shadow.frustum,
            [&](auto& node) { on_intersection(shadow, models[node.index]); },
            [&](auto& node) { not_intersection(shadow, models[node.index]); });
    }
}

void gearoenix::render::record::Lights::update_dynamic_models(Models& models)
{
#if GEAROENIX_RENDER_RECORD_MODEL_BUILD_BVH_FOR_DYNAMICS
    update_models(models.dynamic_models_bvh, models.models);
#else
    const auto models_count = models.models.size();
    if (const auto dynamic_models_count = models_count - models.dynamic_models_starting_index; dynamic_models_count == 0) {
        return;
    }

    const auto begin = models.models.begin() + static_cast<decltype(models.models)::difference_type>(models.dynamic_models_starting_index);
    core::sync::parallel_for(begin, models.models.end(), [&](auto& model, auto) {
        clear_lights(model);
        for (auto& shadow : shadow_caster_directionals) {
            check_intersection(shadow, model);
        }
    });
#endif
}

void gearoenix::render::record::Lights::update_static_models(Models& models)
{
    update_models(models.static_models_bvh, models.models);
}

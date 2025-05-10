#include "gx-rnd-rcd-light.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "gx-rnd-rcd-model.hpp"

void gearoenix::render::record::LightThreadData::clear()
{
    shadow_caster_directionals.clear();
}

gearoenix::render::record::Lights::Lights()
    : threads(core::sync::threads_count())
{
}

void gearoenix::render::record::Lights::update(core::ecs::Entity* const scene_entity)
{
    for (auto& t : threads) {
        t.clear();
    }
    shadow_caster_directionals.clear();

    core::ecs::World::get().parallel_system<core::ecs::All<light::ShadowCasterDirectional, physics::Transformation>>(
        [&](auto* const e, auto* const l, auto* const trn, const auto kernel_index) {
            if (!l->get_enabled() || !trn->get_enabled() || !e->contains_in_parents(scene_entity)) {
                return;
            }

            threads[kernel_index].shadow_caster_directionals.push_back(
                ShadowCasterDirectionalLightData {
                    .frustum = l->get_shadow_frustum()->get_frustum(),
                    .normalised_vp = l->get_shadow_camera()->get_view_projection(),
                    .direction = math::Vec3<float>(l->get_shadow_transform()->get_z_axis()),
                    .colour = l->colour,
                    .entity = e,
                    .shadow_caster_directional = l,
                    .transform = trn,
                });
        });

    for (auto& t : threads) {
        shadow_caster_directionals.insert(
            shadow_caster_directionals.end(),
            std::make_move_iterator(t.shadow_caster_directionals.begin()),
            std::make_move_iterator(t.shadow_caster_directionals.end()));
    }
}

void gearoenix::render::record::Lights::update_models(physics::accelerator::Bvh<Model>& bvh)
{
    using node_t = std::remove_cvref_t<decltype(bvh)>::Data;

    bvh.call_on_all([&](node_t& n) -> void {
        n.user_data.lights.shadow_caster_directionals.clear();
        n.user_data.lights.directionals.clear();
    });

    for (auto& shadow : shadow_caster_directionals) {
        bvh.call_on_intersecting(
            shadow.frustum,
            [&](node_t& node) {
                auto& [directionals, shadow_caster_directionals] = node.user_data.lights;
                if (shadow_caster_directionals.size() == shadow_caster_directionals.max_size()) {
                    if (directionals.size() == directionals.max_size()) {
                        return;
                    }
                    directionals.push_back(ModelDirectionalLight {
                        .direction = shadow.direction, .light = shadow.shadow_caster_directional });
                    return;
                }
                shadow_caster_directionals.push_back(&shadow);
            },
            [&](node_t& node) {
                auto& [directionals, _] = node.user_data.lights;
                if (directionals.size() == directionals.max_size()) {
                    return;
                }
                directionals.push_back(ModelDirectionalLight {
                    .direction = shadow.direction,
                    .light = shadow.shadow_caster_directional,
                });
            });
    }
}

void gearoenix::render::record::Lights::update_dynamic_models(Models& models)
{
    update_models(models.dynamic_models_bvh);
}

void gearoenix::render::record::Lights::update_static_models(Models& models)
{
    update_models(models.static_models_bvh);
}
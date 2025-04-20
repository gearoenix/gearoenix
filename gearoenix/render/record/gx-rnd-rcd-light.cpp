export module gearoenix.render.record.light;

import gearoenix.render.record.model;

#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../math/gx-math-frustum.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../light/gx-rnd-lt-directional.hpp"

namespace gearoenix::render::record {
export struct ShadowCasterDirectionalLightData final {
    math::Frustum<double> frustum;
    math::Mat4x4<float> normalised_vp;
    math::Vec3<float> direction;
    math::Vec3<float> colour;
    core::ecs::Entity* entity = nullptr;
    light::ShadowCasterDirectional* shadow_caster_directional = nullptr;
    physics::Transformation* transform = nullptr;
};

struct ThreadData final {
    std::vector<ShadowCasterDirectionalLightData> shadow_caster_directionals;

    void clear()
    {
        shadow_caster_directionals.clear();
    }
};

export struct Lights final {
    std::vector<ThreadData> threads;
    std::vector<ShadowCasterDirectionalLightData> shadow_caster_directionals;

    Lights()
        : threads(core::sync::threads_count())
    {
    }

    void update(core::ecs::Entity* const scene_entity)
    {
        for (auto& t : threads) {
            t.clear();
        }
        shadow_caster_directionals.clear();

        core::ecs::World::get().parallel_system<core::ecs::All<light::ShadowCasterDirectional, physics::Transformation>>(
            [&](auto* const e, auto* const l, const auto* const trn, const auto kernel_index) {
                if (!l->get_enabled() || !trn->get_enabled() || e->contains_in_parents(scene_entity)) {
                    return;
                }

                threads[kernel_index].lights.emplace_back(
                    l->get_shadow_frustum()->get_frustum(),
                    l->get_shadow_camera()->get_view_projection(),
                    math::Vec3<float>(l->get_shadow_transform()->get_z_axis()),
                    l->colour,
                    e,
                    l,
                    trn);
            });
    }

    void update_models(physics::accelerator::Bvh<Model>& bvh) const
    {
        using node_t = std::remove_cvref_t<decltype(bvh)>::Data;

        bvh.call_on_all([&](node_t& n) -> void {
            n.user_data.lights.shadow_caster_directionals.clear();
            n.user_data.lights.directionals.clear();
        });

        for (const auto& shadow : shadow_caster_directionals) {
            bvh.call_on_intersecting(
                shadow.frustum,
                [&](node_t& node) {
                    auto& [directionals, shadow_caster_directionals] = node.user_data.lights;
                    if (shadow_caster_directionals.size() == shadow_caster_directionals.max_size()) {
                        if (directionals.size() == directionals.max_size()) {
                            return;
                        }
                        directionals.emplace_back(shadow.direction, shadow.shadow_caster_directional);
                        return;
                    }
                    shadow_caster_directionals.emplace_back(&shadow);
                },
                [&](node_t& node) {
                    auto& [directionals, _] = node.user_data.lights;
                    if (directionals.size() == directionals.max_size()) {
                        return;
                    }
                    directionals.emplace_back(shadow.direction, shadow.shadow_caster_directional);
                });
        }
    }

    void update_dynamic_models(Models& models) const
    {
        update_models(models.dynamic_models_bvh);
    }

    void update_static_models(Models& models) const
    {
        update_models(models.static_models_bvh);
    }
};
}
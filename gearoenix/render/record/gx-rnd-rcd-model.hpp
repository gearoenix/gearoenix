export module gearoenix.render.record.model;

import gearoenix.render.record.light;

#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/collider/gx-phs-cld-collider.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../gx-rnd-build-configuration.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../reflection/gx-rnd-rfl-probe.hpp"
#include <boost/container/static_vector.hpp>
#include <boost/functional/hash.hpp>

namespace gearoenix::render::record {
export struct ModelDirectionalLight final {
    math::Vec3<float> direction;
    light::Light* light = nullptr;
};

export struct ModelLight final {
    boost::container::static_vector<ModelDirectionalLight, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directionals;
    boost::container::static_vector<ShadowCasterDirectionalLightData*, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directionals;
};

export struct Model final {
    core::ecs::Entity* entity = nullptr;
    model::Model* model = nullptr;
    physics::animation::Armature* armature = nullptr;
    physics::Transformation* transform = nullptr;
    physics::collider::Collider* collider = nullptr;
    reflection::Probe* probe = nullptr;
    ModelLight lights;
};

struct ThreadData final {
    std::vector<Model> static_models;
    std::vector<Model> dynamic_models;
    std::size_t static_models_hash = 0;

    void clear()
    {
        static_models.clear();
        dynamic_models.clear();
        static_models_hash = 0;
    }
};

export struct Models final {
    std::vector<ThreadData> threads;
    std::vector<Model> static_models;
    std::vector<Model> dynamic_models;
    physics::accelerator::Bvh<Model> static_models_bvh;
    physics::accelerator::Bvh<Model> dynamic_models_bvh;
    std::uintptr_t previous_frame_static_models_hash = 0;

    Models()
        : threads(core::sync::threads_count())
    {
    }

    void update(core::ecs::Entity* const scene_entity)
    {
        for (auto& t : threads) {
            t.clear();
        }

        std::atomic refresh_static_bvh = false;
        core::ecs::World::get().parallel_system<core::ecs::All<model::Model, physics::Transformation, physics::collider::Collider>>(
            [&](auto* const e, auto* const mdl, const auto* const trn, const auto* const cld, const auto kernel_index) {
                if (!mdl->get_enabled() || !trn->get_enabled() || !cld->get_enabled() || e->contains_in_parents(scene_entity)) {
                    return;
                }

                if (mdl->cameras.has_value()) {
                    mdl->cameras_flags = 0;
                    for (const auto& cameras = mdl->cameras.value(); const auto camera_id : cameras) {
                        if (auto camera_search = cameras.find(camera_id); cameras.end() != camera_search) {
                            mdl->cameras_flags |= camera_search->second.cam->get_flag();
                        }
                    }
                } else {
                    mdl->cameras_flags = static_cast<std::uint64_t>(-1);
                }

                auto* const parent_entity = e->get_parent_entity();
                auto* const armature = parent_entity? parent_entity->template get_component<physics::animation::Armature>(): nullptr;

                auto& td = threads[kernel_index];

                if (mdl->get_is_transformable()) {
                    td.dynamic_models.emplace_back(e, mdl, armature, trn, cld);
                } else {
                    td.static_models.emplace_back(e, mdl, armature, trn, cld);
                    if (trn->get_changed()) {
                        refresh_static_bvh = true;
                    }
                    td.static_models_hash = boost::hash_combine(td.static_models_hash, boost::hash_value(e->get_object_id()));
                }
            });

        std::uintptr_t static_models_hash = 0;
        static_models.clear();
        dynamic_models.clear();
        for (auto& td : threads) {
            static_models.insert(
                static_models.end(),
                std::make_move_iterator(td.static_models.begin()),
                std::make_move_iterator(td.static_models.end()));
            dynamic_models.insert(
                dynamic_models.end(),
                std::make_move_iterator(td.dynamic_models.begin()),
                std::make_move_iterator(td.dynamic_models.end()));
            boost::hash_combine(static_models_hash, td.static_models_hash);
        }

        if (refresh_static_bvh || static_models_hash != previous_frame_static_models_hash) {
            GX_LOG_D("Warning, recreation of BVH, do not transform static models,"
                     "if you are seeing this log only once for each scene it is ok,"
                     "otherwise you're going to have performance problem.");
            previous_frame_static_models_hash = static_models_hash;
            static_models_bvh.reset();
            for (const auto& m : static_models) {
                static_models_bvh.add({ m.collider->get_surrounding_box(), m });
            }
            static_models_bvh.create_nodes();
        }

        dynamic_models_bvh.reset();
        for (const auto& m : dynamic_models) {
            dynamic_models_bvh.add({ m.collider->get_surrounding_box(), m });
        }
        dynamic_models_bvh.create_nodes();
    }
};
}
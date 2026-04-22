#include "gx-rnd-rcd-model.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/gx-cr-profiler.hpp"
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/collider/gx-phs-cld-collider.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../model/gx-rnd-mdl-uniform.hpp"
#include "../reflection/gx-rnd-rfl-baked.hpp"
#include "../reflection/gx-rnd-rfl-manager.hpp"
#include "../reflection/gx-rnd-rfl-probe.hpp"

#include <boost/functional/hash.hpp>

gearoenix::render::record::Models::Models() = default;

void gearoenix::render::record::Models::update_after_change(core::ecs::Entity* const scene_entity)
{
    struct ModelThreadData final {
        std::vector<Model> static_models;
        std::vector<Model> dynamic_models;
#if GEAROENIX_RENDER_RECORD_MODEL_DEBUG
        std::size_t models_hash = 0;
#endif
    };

    std::vector<ModelThreadData> threads(core::sync::threads_count());

    auto* const black_probe = reflection::Manager::get().get_black().get();

    core::ecs::World::get().parallel_system<core::ecs::All<model::Model, physics::Transformation, physics::collider::Collider>>([&](auto* const e, auto* const mdl, auto* const trn, auto* const cld, const auto kernel_index) {
        if (!mdl->get_enabled() || !trn->get_enabled() || !cld->get_enabled() || !e->contains_in_parents(scene_entity)) {
            return;
        }

        if (mdl->cameras.has_value()) {
            mdl->cameras_flags = 0;
            for (const auto& cameras = mdl->cameras.value(); const auto camera_id : cameras) {
                mdl->cameras_flags |= camera_id->get_flag();
            }
        } else {
            mdl->cameras_flags = static_cast<std::uint64_t>(-1);
        }

        auto* const parent_entity = e->get_parent();
        auto* const armature = parent_entity ? parent_entity->template get_component<physics::animation::Armature>() : nullptr;
        const auto bones_count = armature ? static_cast<std::uint32_t>(armature->get_all_bones().size()) : 0;
        mdl->get_uniform().template get_ref<GxShaderDataModel>().bones_point_lights_directional_lights_shadow_caster_directional_lights_count.x = bones_count;

        auto& td = threads[kernel_index];

        Model data {
            .model = mdl,
            .collider = cld,
            .probe = black_probe,
        };

        if (mdl->get_is_transformable()) {
            td.dynamic_models.push_back(data);
        } else {
            td.static_models.push_back(data);
        }

#if GEAROENIX_RENDER_RECORD_MODEL_DEBUG
        boost::hash_combine(td.models_hash, boost::hash_value(e->get_object_id()));
        boost::hash_combine(td.models_hash, boost::hash_value(mdl->cameras_flags));
        boost::hash_combine(td.models_hash, boost::hash_value(bones_count));
#endif
    });

    models.clear();
    for (auto& td : threads) {
        models.insert(models.end(), std::make_move_iterator(td.static_models.begin()), std::make_move_iterator(td.static_models.end()));
    }
    dynamic_models_starting_index = models.size();
    for (auto& td : threads) {
        models.insert(models.end(), std::make_move_iterator(td.dynamic_models.begin()), std::make_move_iterator(td.dynamic_models.end()));
    }

    {
        static_models_bvh.reset();
        const auto static_models_count = static_cast<physics::accelerator::Bvh::index_t>(dynamic_models_starting_index);
        for (auto i = decltype(static_models_count) { 0 }; i < static_models_count; ++i) {
            const auto& m = models[i];
            static_models_bvh.add({ m.collider->get_surrounding_box(), i });
        }
        static_models_bvh.create_nodes();
    }

#if GEAROENIX_RENDER_RECORD_MODEL_DEBUG
    previous_models_hash = 0;
    for (auto& td : threads) {
        boost::hash_combine(previous_models_hash, td.models_hash);
    }

    last_frame_update = engine::Engine::get_frame_number_from_start();
#endif
}

void gearoenix::render::record::Models::update_per_frame([[maybe_unused]] core::ecs::Entity* scene_entity)
{
    for (const auto& m : models) {
        m.model->update_uniform();
    }

#if GEAROENIX_RENDER_RECORD_MODEL_DEBUG
    struct ModelThreadData final {
        std::size_t models_hash = 0;
    };

    std::vector<ModelThreadData> threads(core::sync::threads_count());

    core::ecs::World::get().parallel_system<core::ecs::All<model::Model, physics::Transformation, physics::collider::Collider>>([&](auto* const e, auto* const mdl, auto* const trn, auto* const cld, const auto kernel_index) {
        if (!mdl->get_enabled() || !trn->get_enabled() || !cld->get_enabled() || !e->contains_in_parents(scene_entity)) {
            return;
        }

        if (mdl->cameras.has_value()) {
            mdl->cameras_flags = 0;
            for (const auto& cameras = mdl->cameras.value(); const auto camera_id : cameras) {
                mdl->cameras_flags |= camera_id->get_flag();
            }
        } else {
            mdl->cameras_flags = static_cast<std::uint64_t>(-1);
        }

        auto* const parent_entity = e->get_parent();
        auto* const armature = parent_entity ? parent_entity->template get_component<physics::animation::Armature>() : nullptr;
        const auto bones_count = armature ? static_cast<std::uint32_t>(armature->get_all_bones().size()) : 0;

        auto& td = threads[kernel_index];

        GX_ASSERT(mdl->get_is_transformable() || engine::Engine::get().get_frame_number_from_start() == last_frame_update || !trn->get_changed());

        // Static models cannot have transformations!
        // If you desperately want to transform static models, change it to dynamic, then update the scene,
        // then transform it, and then when all of your transformations are finished, change it back to static
        // and then again update the scene.

        boost::hash_combine(td.models_hash, boost::hash_value(e->get_object_id()));
        boost::hash_combine(td.models_hash, boost::hash_value(mdl->cameras_flags));
        boost::hash_combine(td.models_hash, boost::hash_value(bones_count));
    });

    std::uintptr_t models_hash = 0;
    for (auto& td : threads) {
        boost::hash_combine(models_hash, td.models_hash);
    }

    GX_ASSERT(previous_models_hash == models_hash);
#endif

#if GEAROENIX_RENDER_RECORD_MODEL_BUILD_BVH_FOR_DYNAMICS
    {
        GX_PROFILE_SCOPE(render_record_models_dynamic_models_bvh_recreation);
        dynamic_models_bvh.reset();
        const auto dynamic_models_end = static_cast<physics::accelerator::Bvh::index_t>(models.size());
        for (auto i = static_cast<physics::accelerator::Bvh::index_t>(dynamic_models_starting_index); i < dynamic_models_end; ++i) {
            const auto& m = models[i];
            dynamic_models_bvh.add({ m.collider->get_surrounding_box(), i });
        }
        dynamic_models_bvh.create_nodes();
    }
#endif
}
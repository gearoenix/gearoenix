#include "gx-rnd-rcd-model.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/collider/gx-phs-cld-collider.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../reflection/gx-rnd-rfl-baked.hpp"
#include "../reflection/gx-rnd-rfl-manager.hpp"
#include "../reflection/gx-rnd-rfl-probe.hpp"
#include <boost/functional/hash.hpp>

void gearoenix::render::record::ModelThreadData::clear()
{
    static_models.clear();
    dynamic_models.clear();
    static_models_hash = 0;
}

gearoenix::render::record::Models::Models()
    : threads(core::sync::threads_count())
{
}

void gearoenix::render::record::Models::update(core::ecs::Entity* const scene_entity)
{
    for (auto& t : threads) {
        t.clear();
    }

    std::atomic refresh_static_bvh = false;
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

        auto& td = threads[kernel_index];

        Model data {
            .entity = e,
            .model = mdl,
            .armature = armature,
            .transform = trn,
            .collider = cld,
            .probe = black_probe,
            .bones_count = bones_count,
        };

        if (mdl->get_is_transformable()) {
            td.dynamic_models.push_back(data);
        } else {
            td.static_models.push_back(data);
            if (trn->get_changed()) {
                refresh_static_bvh = true;
            }
            boost::hash_combine(td.static_models_hash, boost::hash_value(e->get_object_id()));
        }
    });

    std::uintptr_t static_models_hash = 0;
    static_models.clear();
    dynamic_models.clear();
    for (auto& td : threads) {
        static_models.insert(static_models.end(), std::make_move_iterator(td.static_models.begin()), std::make_move_iterator(td.static_models.end()));
        dynamic_models.insert(dynamic_models.end(), std::make_move_iterator(td.dynamic_models.begin()), std::make_move_iterator(td.dynamic_models.end()));
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
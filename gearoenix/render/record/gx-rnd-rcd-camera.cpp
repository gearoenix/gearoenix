#include "gx-rnd-rcd-camera.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/collider/gx-phs-cld-collider.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../reflection/gx-rnd-rfl-baked.hpp"
#include "../reflection/gx-rnd-rfl-manager.hpp"
#include "gx-rnd-rcd-model.hpp"

#if GX_PLATFORM_WEBASSEMBLY || GX_PLATFORM_APPLE || GX_PLATFORM_ANDROID
#define GX_PARALLEL_POLICY
#else
#include <execution>

#define GX_PARALLEL_POLICY std::execution::par_unseq,
#endif

gearoenix::render::record::Camera::Camera()
    : threads_mvps(core::sync::threads_count())
{
}

void gearoenix::render::record::Camera::clear()
{
    translucent_models.clear();
    opaque_models.clear();
}

void gearoenix::render::record::Camera::update_models(physics::accelerator::Bvh<Model>& bvh)
{
    using node_t = std::remove_cvref_t<decltype(bvh)>::Data;

    const auto camera_location = transform->get_global_position();
    auto* const black_probe = reflection::Manager::get().get_black().get();

    bvh.call_on_intersecting(*collider, [&, this](node_t& node) -> void {
        if ((node.user_data.model->cameras_flags & camera->get_flag()) == 0) {
            return;
        }
        const auto dir = camera_location - node.box.get_center();
        const auto dis = dir.square_length();
        auto& m = node.user_data;
        if (parent_reflection_probe == m.probe) {
            m.probe = black_probe;
        }
        if (m.model->has_transparent_material()) {
            translucent_models.emplace_back(dis, &m);
        } else {
            opaque_models.emplace_back(dis, &m);
        }
    });
}

void gearoenix::render::record::Camera::update_models(Models& models)
{
    clear();

    const auto target_dimension = camera->get_target().get_dimension();
    viewport_clip = camera->get_starting_clip_ending_clip() * math::Vec4<float>(target_dimension, target_dimension);
    skybox_scale = camera->get_far() * (1.0f / 1.7320508075688772935274463415059f);

    parent_entity = entity->get_parent();
    parent_reflection_probe = parent_entity ? parent_entity->get_component<reflection::Probe>() : nullptr;

    update_models(models.static_models_bvh);
    update_models(models.dynamic_models_bvh);

    std::sort(GX_PARALLEL_POLICY opaque_models.begin(), opaque_models.end(),
        [](const auto& rhs, const auto& lhs) { return rhs.first < lhs.first; });
    std::sort(GX_PARALLEL_POLICY translucent_models.begin(), translucent_models.end(),
        [](const auto& rhs, const auto& lhs) { return rhs.first > lhs.first; });

    mvps.clear();
    auto gather_mvps = [&](auto& trn_opq_models) {
        for (auto& t: threads_mvps) {
            t.clear();
        }
        core::sync::parallel_for_i(trn_opq_models, [&](const auto& d_m, const auto i, const auto ki) {
            const auto& rm = *d_m.second.model;
            if (const auto& m = *rm.model; !m.needs_mvp() && camera->get_usage() != camera::Camera::Usage::Shadow) {
                return;
            }
            if (rm.armature) {
                for (auto* const bone : rm.armature->get_all_bones()) {
                    threads_mvps[ki].emplace_back(i, camera->get_view_projection() * math::Mat4x4<float>(bone->get_global_matrix()));
                }
            } else {
                threads_mvps[ki].emplace_back(i, camera->get_view_projection() * math::Mat4x4<float>(rm.transform->get_global_matrix()));
            }
        });
        auto current_model_index = static_cast<std::uint32_t>(-1);
        for (auto& ms : threads_mvps) {
            for (auto& [i, m] : ms) {
                auto& cam_model = trn_opq_models[i].second;
                if (current_model_index != i) {
                    cam_model.first_mvp_index = static_cast<std::uint32_t>(mvps.size());
                    cam_model.mvps_count = 1;
                    current_model_index = i;
                } else {
                    ++cam_model.mvps_count;
                }
                mvps.push_back(m);
            }
        }
    };

    mvps.clear();

    gather_mvps(opaque_models);
    gather_mvps(translucent_models);
}

void gearoenix::render::record::Cameras::update(core::ecs::Entity* const scene_entity)
{
    for (int i = 0; i < last_camera_index; ++i) {
        cameras[i].clear();
    }

    last_camera_index = 0;
    indices_map.clear();
    mains.clear();
    shadow_casters.clear();
    reflections.clear();

    std::uint64_t flag = 1;
    core::ecs::World::get().synchronised_system<core::ecs::All<camera::Camera, physics::Transformation, physics::collider::Collider>>(
        [&](auto* const e, auto* const cam, auto* const trn, auto* const cld) {
            if (!cam->get_enabled() || !trn->get_enabled() || !cld->get_enabled() || !e->contains_in_parents(scene_entity)) {
                return;
            }
            auto& cam_ref = cameras[last_camera_index];
            cam_ref.entity = e;
            cam_ref.camera = cam;
            cam_ref.transform = trn;
            cam_ref.collider = cld;

            indices_map.emplace(e, last_camera_index);
            switch (cam->get_usage()) {
            case camera::Camera::Usage::Main: {
                mains.emplace(e, last_camera_index);
                break;
            }
            case camera::Camera::Usage::Shadow: {
                shadow_casters.emplace(e, last_camera_index);
                break;
            }
            case camera::Camera::Usage::ReflectionProbe: {
                reflections.emplace(e, last_camera_index);
                break;
            }
            default:
                GX_UNEXPECTED;
            }
            cam->set_flag(flag);

            flag <<= 1;
            ++last_camera_index;
        });
}

void gearoenix::render::record::Cameras::update_models(Models& models)
{
    if (last_camera_index > 0) {
        core::sync::parallel_for(cameras.begin(), cameras.begin() + last_camera_index, [&](auto& c, const auto) {
            c.update_models(models);
        });
    }
}
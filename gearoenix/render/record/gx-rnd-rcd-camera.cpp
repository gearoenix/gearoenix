#include "gx-rnd-rcd-camera.hpp"
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/gx-cr-profiler.hpp"
#include "../../core/sync/gx-cr-sync-thread.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/collider/gx-phs-cld-collider.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../buffer/gx-rnd-buf-manager.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-uniform.hpp"
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

namespace {
void on_intersection(auto& m, auto& self, const auto& camera_location, const auto& m_box, auto* const black_probe, auto& temp_models, auto& all_models)
{
    if ((m.model->cameras_flags & self.camera->get_flag()) == 0) {
        return;
    }
    const auto dir = camera_location - m_box.get_center();
    const auto dis = dir.square_length();
    if (self.parent_reflection_probe == m.probe) {
        m.probe = black_probe;
    }
    if (m.model->has_transparent_material()) {
        temp_models.emplace_back(dis, &m);
    } else {
        all_models.emplace_back(dis, &m);
    }
}

std::atomic<std::size_t> last_mvp_index { 0 };
std::atomic<std::size_t> mvp_uniform_buffer_frame_pointer { 0 };
}

GxShaderDataCameraJointModel& gearoenix::render::record::CameraModel::get_first_mvp() const
{
    return buffer::Manager::get_uniform_region_ptr<GxShaderDataCameraJointModel>(buffer::UniformRegionIndex::camera_joint_model)[first_mvp_index];
}

gearoenix::render::record::Camera::Camera()
    :
#if !GEAROENIX_RENDER_RECORD_MODEL_BUILD_BVH_FOR_DYNAMICS
    threads_temp_models(core::sync::threads_count())
    , threads_all_models(core::sync::threads_count())
#endif
{
}

void gearoenix::render::record::Camera::clear()
{
    temp_models.clear();
    all_models.clear();

#if !GEAROENIX_RENDER_RECORD_MODEL_BUILD_BVH_FOR_DYNAMICS
    for (auto& t : threads_temp_models) {
        t.clear();
    }
    for (auto& t : threads_all_models) {
        t.clear();
    }
#endif
}

void gearoenix::render::record::Camera::update_models(physics::accelerator::Bvh& bvh, std::vector<Model>& models)
{
    const auto camera_location = transform->get_global_position();
    auto* const black_probe = reflection::Manager::get().get_black().get();

    bvh.call_on_intersecting(*collider, [&](auto& node) -> void {
        auto& m = models[node.index];
        on_intersection(m, *this, camera_location, node.box, black_probe, temp_models, all_models);
    });
}

void gearoenix::render::record::Camera::update_models(Models& models)
{
    clear();

    const auto target_dimension = camera->get_target().get_dimension();
    viewport_clip = camera->get_starting_clip_ending_clip() * math::Vec4(target_dimension.to<float>(), target_dimension.to<float>());
    skybox_scale = camera->get_far() * (1.0f / 1.7320508075688772935274463415059f);

    parent_entity = entity->get_parent();
    parent_reflection_probe = parent_entity ? parent_entity->get_component<reflection::Probe>() : nullptr;

    GX_PROFILE_EXP(update_models(models.static_models_bvh, models.models));

#if GEAROENIX_RENDER_RECORD_MODEL_BUILD_BVH_FOR_DYNAMICS
    GX_PROFILE_EXP(update_models(models.dynamic_models_bvh, models.models));
#else
    if (models.dynamic_models_starting_index < models.models.size()) {
        const auto camera_location = transform->get_global_position();
        auto* const black_probe = reflection::Manager::get().get_black().get();
        const auto models_begin = models.models.begin() + static_cast<decltype(models.models)::difference_type>(models.dynamic_models_starting_index);
        core::sync::parallel_for(models_begin, models.models.end(), [&](auto& model, const auto ki) {
            const auto& m_box = model.collider->get_surrounding_box();
            if (!collider->check_intersection(m_box)) {
                return;
            }
            on_intersection(model, *this, camera_location, m_box, black_probe, threads_temp_models[ki], threads_all_models[ki]);
        });

        for (auto& t : threads_temp_models) {
            temp_models.insert(temp_models.end(), t.begin(), t.end());
        }
        for (auto& t : threads_all_models) {
            all_models.insert(all_models.end(), t.begin(), t.end());
        }
    }
#endif

    GX_PROFILE_EXP(std::sort(GX_PARALLEL_POLICY all_models.begin(), all_models.end(), [](const auto& rhs, const auto& lhs) {
        return rhs.first < lhs.first;
    }));
    GX_PROFILE_EXP(std::sort(GX_PARALLEL_POLICY temp_models.begin(), temp_models.end(), [](const auto& rhs, const auto& lhs) {
        return rhs.first > lhs.first;
    }));
    translucent_models_starting_index = all_models.size();
    GX_PROFILE_EXP(all_models.insert(all_models.end(), temp_models.begin(), temp_models.end()));

    const auto gather_mvps = [&](std::vector<std::pair<core::fp_t, CameraModel>>& trn_opq_models) {
        struct Context {
            std::vector<std::pair<core::fp_t, CameraModel>>& models;
            camera::Camera& camera;
        } ctx { trn_opq_models, *camera };

        core::sync::parallel(+[](const unsigned int kernel_index, const unsigned int kernels_count, void* const context) {
            auto& [mds, cmr] = *static_cast<Context*>(context);
            const auto mds_count = static_cast<unsigned int>(mds.size());
            std::size_t mvps_count = 0;
            const auto is_shadow = cmr.get_usage() == camera::Camera::Usage::Shadow;
            for (auto i = kernel_index; i < mds_count; i += kernels_count) {
                const auto& m = *mds[i].second.model->model;
                if (!(m.needs_mvp() || is_shadow)) {
                    continue;
                }
                if (m.get_armature()) {
                     mvps_count += m.get_armature()->get_all_bones().size();
                } else {
                    ++mvps_count;
                }
            }
            auto mvp_index = last_mvp_index.fetch_add(mvps_count, std::memory_order_relaxed);
            GX_ASSERT_D(mvp_index + mvps_count <= gearoenix::render::record::Camera::cameras_joint_models_max_count);
            const auto ptr_i = mvp_uniform_buffer_frame_pointer.load(std::memory_order_relaxed) + mvp_index * sizeof(GxShaderDataCameraJointModel);
            auto ptr = reinterpret_cast<GxShaderDataCameraJointModel*>(ptr_i);
            for (auto i = kernel_index; i < mds_count; i += kernels_count) {
                auto& cam_m = mds[i].second;
                auto& m = *cam_m.model->model;
                if (!(m.needs_mvp() || is_shadow)) {
                    continue;
                }
                cam_m.first_mvp_index = static_cast<std::uint32_t>(mvp_index);
                if (m.get_armature()) {
                    const auto bones_count = m.get_armature()->get_all_bones().size();
                    m.update_bones_uniform(cmr.get_view_projection(), ptr);
                    ptr += bones_count;
                    mvp_index += bones_count;
                } else {
                    ptr->mvp = cmr.get_view_projection() * m.get_transform()->get_global_matrix();
                    ++ptr;
                    ++mvp_index;
                }
            } }, &ctx);
    };

    GX_PROFILE_EXP(gather_mvps(all_models));

    GX_ASSERT_D(last_mvp_index.load(std::memory_order_relaxed) <= cameras_joint_models_max_count);
}

void gearoenix::render::record::Cameras::update(core::ecs::Entity* const scene_entity)
{
    for (int i = 0; i < last_camera_index; ++i) {
        cameras[i].clear();
    }

    last_mvp_index.store(0, std::memory_order_relaxed);
    mvp_uniform_buffer_frame_pointer.store(
        buffer::Uniform::get_frame_pointer() + static_cast<std::size_t>(buffer::Manager::get_region(buffer::UniformRegionIndex::camera_joint_model)->get_offset()),
        std::memory_order_relaxed);

    last_camera_index = 0;
    indices_map.clear();
    mains.clear();
    shadow_casters.clear();
    reflections.clear();

    std::uint64_t flag = 1;
    core::ecs::World::get().synchronised_system<core::ecs::All<camera::Camera, physics::Transformation, physics::collider::Collider>>([&](auto* const e, auto* const cam, auto* const trn, auto* const cld) {
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

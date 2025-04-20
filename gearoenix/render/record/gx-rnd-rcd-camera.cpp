#include "gx-rnd-rcd-camera.hpp"

namespace gearoenix::render::record {
export struct Bloom final {
    math::Vec4<float> scatter_clamp_max_threshold_threshold_knee;
};

export struct Camera final {
    core::ecs::Entity* entity = nullptr;
    core::ecs::Entity* parent_entity = nullptr;
    reflection::Probe* parent_reflection_probe = nullptr;
    camera::Camera* camera = nullptr;
    physics::Transformation* transform = nullptr;
    physics::collider::Collider* collider = nullptr;
    math::Mat4x4<float> view_projection;
    math::Vec3<float> position;
    std::optional<Bloom> bloom = std::nullopt;
    std::vector<std::pair<double, Model*>> translucent_models;
    std::vector<std::pair<double, Model*>> all_models;
    std::vector<math::Mat4x4<float>> mvps;
    std::vector<std::vector<std::pair<std::uint32_t, math::Mat4x4<float>>>> threads_mvps;

    void clear()
    {
        translucent_models.clear();
        all_models.clear();
    }

    void update_models(physics::accelerator::Bvh<Model>& bvh)
    {
        using node_t = std::remove_cvref_t<decltype(bvh)>::Data;

        bvh.call_on_intersecting(*collider, [&, this](node_t& node)
        {
            if ((node.user_data.model->cameras_flags & camera->get_flag()) == 0) {
                return;
            }
            const auto dir = transform->get_local_position() - node.box.get_center();
            const auto dis = dir.square_length();
            auto& m = node.user_data;
            if (parent_reflection_probe == m.probe) {
                m.probe = nullptr;
            }
            if (m.model->has_transparent_material()) {
                translucent_models.emplace_back(dis, &m);
            } else {
                all_models.emplace_back(dis, &m);
            }
        });
    }

    void update_models(Models& models)
    {
        clear();

        const auto camera_location = transform->get_global_position();
        update_models(models.static_models_bvh);
        update_models(models.dynamic_models_bvh);

        std::sort(std::execution::par_unseq, all_models.begin(), all_models.end(),
            [](const auto& rhs, const auto& lhs) { return rhs.first < lhs.first; });
        std::sort(std::execution::par_unseq, translucent_models.begin(), translucent_models.end(),
            [](const auto& rhs, const auto& lhs) { return rhs.first > lhs.first; });

        all_models.insert(
            all_models.end(),
            std::make_move_iterator(translucent_models.begin()),
            std::make_move_iterator(translucent_models.end()));

        core::sync::ParallelFor::exec(all_models.begin(),all_models.end(), [&](const auto& d_m, const auto) {
            auto* const m = d_m.second;
            if (!m->needs_mvp() && camera->get_usage() != camera::Camera::Usage::Shadow) {
                return;
            }
            if (m.armature) {
                for (auto*const bone: m.armature->get_bones()) {
                    auto& bone_data = scene_data.bones_data[i];
                    camera_data.threads_mvps[kernel_index].emplace_back(index, camera_data.vp * bone_data.m);
                }
            } else {
                camera_data.threads_mvps[kernel_index].emplace_back(index, camera_data.vp * m.m);
            }
        });
        auto current_model_index = static_cast<std::uint32_t>(-1);
        for (auto& ms : camera_data.threads_mvps) {
            for (auto& im : ms) {
                if (current_model_index != im.first) {
                    camera_data.models_data[im.first].second.first_mvp_index = static_cast<std::uint32_t>(camera_data.mvps.size());
                    current_model_index = im.first;
                }
                camera_data.mvps.push_back(im.second);
            }
        }

        core::sync::ParallelFor::exec(
            scene_data.debug_mesh_data.begin(),
            scene_data.debug_mesh_data.end(),
            [&](DebugModel m, const unsigned int kernel_index) {
                m.m = camera_data.vp * m.m;
                camera_data.debug_meshes_threads[kernel_index].push_back(std::move(m));
            });
        for (auto& v : camera_data.debug_meshes_threads) {
            std::ranges::move(v, std::back_inserter(camera_data.debug_meshes));
        }
    }
};

export struct Cameras final {
    constexpr static auto cameras_count = 64;
    typedef core::static_flat_map<core::ecs::Entity*, int, cameras_count> camera_map_t;

    int last_camera_index = 0;
    std::array<Camera, cameras_count> cameras;
    camera_map_t indices_map;
    camera_map_t mains;
    camera_map_t shadow_casters;
    camera_map_t reflections;

    void update(core::ecs::Entity* const scene_entity)
    {
        for (int i = 0; i < last_camera_index; ++i) { cameras[i].clear(); }

        last_camera_index = 0;
        indices_map.clear();
        std::uint64_t flag = 1;
        core::ecs::World::get().synchronised_system<core::ecs::All<camera::Camera, physics::Transformation, physics::collider::Collider>>(
            [&](auto* const e, auto* const cam, auto* const trn, auto* const cld) {
                if (!cam->get_enabled() || !trn->get_enabled() || !cld->get_enabled() || !e->contains_in_parents(scene_entity)) {
                    return;
                }
                auto& cam_ref = cameras[last_camera_index];
                cam_ref.entity = e;
                cam_ref.parent_entity = e->get_parent();
                cam_ref.parent_reflection_probe = e->get_entity()? e->get_entity()->template get_component<reflection::Probe>(): nullptr;
                cam_ref.camera = cam;
                cam_ref.transform = trn;
                cam_ref.collider = cld;
                ////////////////// TODO.........................................................
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

    void update_models(Models& models)
    {
        core::sync::ParallelFor::exec(
            cameras.begin(), cameras.begin() + last_camera_index,
            [&](auto& c, const auto)
            {
                c.update_models(models);
            });
    }
};
}
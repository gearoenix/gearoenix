#pragma once
#include "../../core/gx-cr-static-flat-map.hpp"
#include "../../core/gx-cr-static-flat-set.hpp"
#include "../../math/gx-math-matrix-4d.hpp"

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::accelerator {
template <typename T>
struct Bvh;
}

namespace gearoenix::physics::collider {
struct Collider;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::reflection {
struct Probe;
}

namespace gearoenix::render::record {
struct Model;
struct Models;

struct CameraModel final {
    Model* model = nullptr;
    std::uint32_t first_mvp_index = static_cast<std::uint32_t>(-1);
    std::uint32_t mvps_count = static_cast<std::uint32_t>(-1);
};

struct Bloom final {
    math::Vec4<float> scatter_clamp_max_threshold_threshold_knee;
};

struct Camera final {
    static constexpr auto cameras_joint_models_max_count = 1024 * 32;

    math::Vec4<float> viewport_clip;
    /// This field is used for scaling the skybox to the far end and at the same time preventing clipping
    float skybox_scale = 1.0f;
    core::ecs::Entity* entity = nullptr;
    core::ecs::Entity* parent_entity = nullptr;
    reflection::Probe* parent_reflection_probe = nullptr;
    camera::Camera* camera = nullptr;
    physics::Transformation* transform = nullptr;
    physics::collider::Collider* collider = nullptr;
    std::vector<std::pair<double, CameraModel>> translucent_models;
    std::vector<std::pair<double, CameraModel>> all_models;
    std::vector<math::Mat4x4<float>> mvps;
    std::vector<std::vector<std::pair<std::uint32_t, math::Mat4x4<float>>>> threads_mvps;

    Camera();
    void clear();
    void update_models(physics::accelerator::Bvh<Model>& bvh);
    void update_models(Models& models);
};

struct Cameras final {
    constexpr static auto cameras_count = 64;
    typedef core::static_flat_map<core::ecs::Entity*, int, cameras_count> camera_map_t;

    int last_camera_index = 0;
    std::array<Camera, cameras_count> cameras;
    camera_map_t indices_map;
    camera_map_t mains;
    camera_map_t shadow_casters;
    camera_map_t reflections;

    void update(core::ecs::Entity* scene_entity);
    void update_models(Models& models);
};
}
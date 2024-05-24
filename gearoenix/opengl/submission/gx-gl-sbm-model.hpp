#ifndef GEAROENIX_GL_SUBMISSION_MODEL_HPP
#define GEAROENIX_GL_SUBMISSION_MODEL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../gx-gl-types.hpp"
#include <array>

namespace gearoenix::physics {
struct TransformationComponent;
}

namespace gearoenix::physics::animation {
struct Armature;
}

namespace gearoenix::physics::collider {
struct Aabb3;
}

namespace gearoenix::gl {
struct Camera;
struct Engine;
struct Model;
}

namespace gearoenix::gl::submission {
struct Camera;
struct Scene;
struct Model final {
    math::Mat4x4<float> m;
    math::Mat4x4<float> inv_m;
    std::size_t first_mesh_index = static_cast<std::size_t>(-1);
    std::size_t last_mesh_index = static_cast<std::size_t>(-1);
    std::size_t first_bone_index = static_cast<std::size_t>(-1);
    std::size_t bones_count = 0;
    std::size_t directional_lights_count = 0;
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directional_lights_direction {};
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directional_lights_colour {};
    std::size_t shadow_caster_directional_lights_count = 0;
    std::array<math::Mat4x4<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_normalised_vp {};
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_direction {};
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_colour {};
    std::array<uint, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_shadow_map_texture {};
    // Reflection probe data
    uint irradiance = static_cast<uint>(-1);
    uint radiance = static_cast<uint>(-1);
    float radiance_lod_coefficient = 0.0;
    double reflection_probe_size = std::numeric_limits<double>::max();
    std::size_t first_mvp_index = static_cast<std::size_t>(-1); // It is used for shadow
    const char* name = nullptr;

    Model();
    Model(
        Engine& e, gl::Model* model, physics::TransformationComponent* model_transform, Scene& scene,
        physics::animation::Armature* armature);
    [[nodiscard]] bool has_transparent_material(const Scene& scene) const;
    [[nodiscard]] bool needs_mvp(const Scene& scene) const;
    void render_shadow(const Scene& scene, const Camera& camera, uint& current_shader) const;
    void render_forward(const Scene& scene, const Camera& camera, uint& current_shader) const;
    void render_deferred_gbuffers(const Scene& scene, const Camera& camera, uint& current_shader) const;
};

struct BvhNodeModel final {
    std::uint64_t cameras_flags = static_cast<std::uint64_t>(-1);
    Model model;

    BvhNodeModel();
    BvhNodeModel(
        Engine& e, gl::Model* gl_model, physics::TransformationComponent* model_transform, Scene& scene,
        physics::animation::Armature* armature);
};

struct DynamicModel final {
    BvhNodeModel base;
    math::Aabb3<double> box;

    DynamicModel(
        Engine& e, gl::Model* gl_model, physics::TransformationComponent* model_transform, Scene& scene,
        physics::animation::Armature* armature, physics::collider::Aabb3* collider);
};

struct DebugModel final {
    math::Mat4x4<float> m;
    math::Vec4<float> colour;
    uint vertex_object = 0;
    sizei indices_count = 0;

    DebugModel(gl::Camera* camera, physics::TransformationComponent* transform);
};
}
#endif
#endif
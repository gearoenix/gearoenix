#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../gx-gl-types.hpp"
#include <array>

namespace gearoenix::physics {
struct Transformation;
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
    std::uint32_t first_mesh_index = static_cast<std::uint32_t>(-1);
    std::uint32_t last_mesh_index = static_cast<std::uint32_t>(-1);
    std::uint32_t first_bone_index = static_cast<std::uint32_t>(-1);
    std::uint32_t bones_count = 0;
    std::uint32_t directional_lights_count = 0;
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directional_lights_direction {};
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directional_lights_colour {};
    std::uint32_t shadow_caster_directional_lights_count = 0;
    std::array<math::Mat4x4<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_normalised_vp {};
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_direction {};
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_colour {};
    std::array<uint, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_shadow_map_texture {};
    // Reflection probe data
    uint irradiance = static_cast<uint>(-1);
    uint radiance = static_cast<uint>(-1);
    float radiance_lod_coefficient = 0.0;
    double reflection_probe_size = std::numeric_limits<double>::max();
    std::uint32_t first_mvp_index = static_cast<std::uint32_t>(-1); // It is used for shadow
    const char* name = nullptr;

    Model();
    Model(const gl::Model*, const physics::Transformation*, Scene&, const physics::animation::Armature*);
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
        const gl::Model* gl_model, const physics::Transformation* model_transform, Scene& scene, const physics::animation::Armature* armature);
};

struct DynamicModel final {
    BvhNodeModel base;
    math::Aabb3<double> box;

    DynamicModel(
        const gl::Model* gl_model, const physics::Transformation* model_transform, Scene& scene,
        const physics::animation::Armature* armature, const physics::collider::Aabb3* collider);
};

struct DebugModel final {
    math::Mat4x4<float> m;
    math::Vec4<float> colour;
    uint vertex_object = 0;
    sizei indices_count = 0;

    DebugModel(const gl::Camera* camera, const physics::Transformation* transform);
};
}
#endif
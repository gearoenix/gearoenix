#ifndef GEAROENIX_GL_SUBMISSION_MODEL_HPP
#define GEAROENIX_GL_SUBMISSION_MODEL_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../gx-gl-types.hpp"
#include <array>

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::gl::material {
struct Material;
}

namespace gearoenix::gl::submission {
struct Model final {
    math::Mat4x4<float> m;
    math::Mat4x4<float> inv_m;
    material::Material* material = nullptr;
    render::material::Material* render_material = nullptr;
    std::size_t directional_lights_count = 0;
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directional_lights_direction;
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directional_lights_colour;
    std::size_t shadow_caster_directional_lights_count = 0;
    std::array<math::Mat4x4<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_normalised_vp;
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_direction;
    std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_colour;
    std::array<uint, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> shadow_caster_directional_lights_shadow_map_texture;
    uint vertex_object = 0;
    sizei indices_count = 0;
    // Reflection probe data
    uint irradiance = static_cast<uint>(-1);
    uint radiance = static_cast<uint>(-1);
    float radiance_lod_coefficient = 0.0;
    double reflection_probe_size = std::numeric_limits<double>::max();
    std::size_t bones_count = 0;
    std::size_t first_bone_index = 0;
    std::size_t fist_mvp_index = 0; // It is used for shadow
};

struct BvhNodeModel final {
    std::uint64_t cameras_flags = static_cast<std::uint64_t>(-1);
    Model model;
};

struct DynamicModel final {
    BvhNodeModel base;
    math::Aabb3<double> box;
};

struct DebugModel final {
    math::Mat4x4<float> m;
    math::Vec4<float> colour;
    uint vertex_object = 0;
    sizei indices_count = 0;
};
}
#endif
#endif
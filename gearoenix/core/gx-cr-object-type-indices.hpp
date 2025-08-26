#pragma once
#include <cstdint>

namespace gearoenix::core {
typedef std::uint32_t object_type_index_t;
typedef std::uint32_t object_id_t;

constexpr auto invalid_object_id = static_cast<object_id_t>(-1);
}

/// For all types which are derived from @code gearoenix::core::ecs::Component @endcode or they have a derived class that is derived from Component.
constexpr auto gearoenix_core_component_type_index_start = static_cast<gearoenix::core::object_type_index_t>(0);

constexpr auto gearoenix_gl_camera_type_index = gearoenix_core_component_type_index_start + 1;
constexpr auto gearoenix_gl_light_shadow_caster_directional_type_index = gearoenix_core_component_type_index_start + 2;
constexpr auto gearoenix_gl_model_type_index = gearoenix_core_component_type_index_start + 3;
constexpr auto gearoenix_gl_reflection_probe_type_index = gearoenix_core_component_type_index_start + 4;
constexpr auto gearoenix_gl_reflection_baked_type_index = gearoenix_core_component_type_index_start + 5;
constexpr auto gearoenix_gl_reflection_runtime_type_index = gearoenix_core_component_type_index_start + 6;
constexpr auto gearoenix_gl_scene_type_index = gearoenix_core_component_type_index_start + 7;
constexpr auto gearoenix_gl_skybox_type_index = gearoenix_core_component_type_index_start + 8;
constexpr auto gearoenix_physics_animation_bone_type_index = gearoenix_core_component_type_index_start + 9;
constexpr auto gearoenix_physics_animation_armature_type_index = gearoenix_core_component_type_index_start + 10;
constexpr auto gearoenix_physics_animation_player_type_index = gearoenix_core_component_type_index_start + 11;
constexpr auto gearoenix_physics_collider_type_index = gearoenix_core_component_type_index_start + 12;
constexpr auto gearoenix_physics_collider_aabb3_type_index = gearoenix_core_component_type_index_start + 13;
constexpr auto gearoenix_physics_collider_frustum_type_index = gearoenix_core_component_type_index_start + 14;
constexpr auto gearoenix_physics_constraint_type_index = gearoenix_core_component_type_index_start + 15;
constexpr auto gearoenix_physics_constraint_jet_controller_type_index = gearoenix_core_component_type_index_start + 16;
constexpr auto gearoenix_physics_transformation_type_index = gearoenix_core_component_type_index_start + 17;
constexpr auto gearoenix_render_camera_type_index = gearoenix_core_component_type_index_start + 18;
constexpr auto gearoenix_render_light_directional_type_index = gearoenix_core_component_type_index_start + 19;
constexpr auto gearoenix_render_light_point_type_index = gearoenix_core_component_type_index_start + 20;
constexpr auto gearoenix_render_light_shadow_caster_directional_type_index = gearoenix_core_component_type_index_start + 21;
constexpr auto gearoenix_render_light_type_index = gearoenix_core_component_type_index_start + 22;
constexpr auto gearoenix_render_model_type_index = gearoenix_core_component_type_index_start + 23;
constexpr auto gearoenix_render_reflection_baked_type_index = gearoenix_core_component_type_index_start + 24;
constexpr auto gearoenix_render_reflection_probe_type_index = gearoenix_core_component_type_index_start + 25;
constexpr auto gearoenix_render_reflection_runtime_type_index = gearoenix_core_component_type_index_start + 26;
constexpr auto gearoenix_render_scene_type_index = gearoenix_core_component_type_index_start + 27;
constexpr auto gearoenix_render_skybox_type_index = gearoenix_core_component_type_index_start + 28;

constexpr auto gearoenix_last_component_type_index = gearoenix_core_component_type_index_start + 29;

/// For all other types
constexpr auto gearoenix_object_type_index_start = static_cast<gearoenix::core::object_type_index_t>(128);

constexpr auto gearoenix_core_ecs_entity_type_index = gearoenix_object_type_index_start + 1;
constexpr auto gearoenix_gl_material_type_index = gearoenix_object_type_index_start + 2;
constexpr auto gearoenix_gl_material_pbr_type_index = gearoenix_object_type_index_start + 3;
constexpr auto gearoenix_gl_material_sprite_type_index = gearoenix_object_type_index_start + 4;
constexpr auto gearoenix_gl_material_unlit_type_index = gearoenix_object_type_index_start + 5;
constexpr auto gearoenix_physics_animation_type_index = gearoenix_object_type_index_start + 6;
constexpr auto gearoenix_physics_animation_armature_animation_type_index = gearoenix_object_type_index_start + 7;
constexpr auto gearoenix_physics_animation_sprite_animation_type_index = gearoenix_object_type_index_start + 8;
constexpr auto gearoenix_render_material_type_index = gearoenix_object_type_index_start + 9;
constexpr auto gearoenix_render_material_pbr_type_index = gearoenix_object_type_index_start + 10;
constexpr auto gearoenix_render_material_sprite_type_index = gearoenix_object_type_index_start + 11;
constexpr auto gearoenix_render_material_unlit_type_index = gearoenix_object_type_index_start + 12;

constexpr auto gearoenix_last_object_type_index = gearoenix_core_component_type_index_start + 13;
#pragma once
#include "gx-phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
struct Aabb3 final : Collider {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = 8192;
    constexpr static auto object_type_index = gearoenix_physics_collider_aabb3_type_index;
    constexpr static std::array all_parent_object_type_indices { Collider::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Collider::object_type_index };

    GX_GET_CREF_PRV(math::Aabb3<double>, original_box);

    void update_surrounding_box() override;

    Aabb3(core::ecs::Entity* entity, std::shared_ptr<Transformation>&& transform, const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name);
    Aabb3(core::ecs::Entity* entity, std::shared_ptr<Transformation>&& transform, const math::Aabb3<double>& original_box, std::string&& name);

public:
    ~Aabb3() override;
    void draw_gizmo() override;
};
}
#pragma once
#include "gx-phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
struct Aabb3 final : Collider {
    constexpr static std::uint32_t MAX_COUNT = 8192;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 1;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES { Collider::TYPE_INDEX };
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES { Collider::TYPE_INDEX };

    GX_GET_CREF_PRV(math::Aabb3<double>, original_box);

    void update_surrounding_box() override;

public:
    Aabb3(
        std::shared_ptr<Transformation>&& transform,
        const math::Vec3<double>& upper,
        const math::Vec3<double>& lower,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    Aabb3(
        std::shared_ptr<Transformation>&& transform,
        const math::Aabb3<double>& original_box,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    ~Aabb3() override;
    void draw_gizmo() override;
};
}
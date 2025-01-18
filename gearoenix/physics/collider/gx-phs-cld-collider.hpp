#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../render/gizmo/gx-rnd-gzm-drawer.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::collider {
struct Collider : core::ecs::Component, render::gizmo::Drawer {
    constexpr static std::uint32_t MAX_COUNT = 8192;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 30;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_CREF_PRT(std::shared_ptr<Transformation>, transform);
    GX_GET_CREF_PRT(math::Aabb3<double>, surrounding_box);
    GX_GET_VAL_PRT(bool, surrounding_box_changed, false);

    Collider(
        std::shared_ptr<Transformation>&& transform,
        core::ecs::component_index_t final_type_index,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    void show_debug_gui() override;

public:
    ~Collider() override;
    virtual void update_surrounding_box() = 0;
    [[nodiscard]] virtual bool check_intersection(const math::Aabb3<double>& box) const;
    [[nodiscard]] virtual math::IntersectionStatus check_intersection_status(const math::Aabb3<double>& box) const;
    static void update_all_after_transform_update();
    void draw_gizmo() override;
};
}
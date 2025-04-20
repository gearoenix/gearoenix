#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../render/gizmo/gx-rnd-gzm-drawer.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::collider {
struct Collider : core::ecs::Component, render::gizmo::Drawer {
    constexpr static auto max_count = 8192;
    constexpr static auto object_type_index = gearoenix_physics_collider_type_index;

    GX_GET_CREF_PRT(std::shared_ptr<Transformation>, transform);
    GX_GET_CREF_PRT(math::Aabb3<double>, surrounding_box);
    GX_GET_VAL_PRT(bool, surrounding_box_changed, false);

    Collider(std::shared_ptr<Transformation>&& transform, core::object_type_index_t final_type_index, std::string&& name);
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
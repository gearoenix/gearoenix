#include "gx-phs-cld-collider.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../render/gizmo/gx-rnd-gzm-manager.hpp"
#include "../../render/imgui/gx-rnd-imgui-math-aabb.hpp"
#include "../../render/imgui/gx-rnd-imgui-type-tree.hpp"
#include "../gx-phs-transformation.hpp"

gearoenix::physics::collider::Collider::Collider(
    std::shared_ptr<Transformation>&& transform,
    const core::ecs::component_index_t final_type_index,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Component(final_type_index, std::move(name), entity_id)
    , transform(std::move(transform))
{
}

void gearoenix::physics::collider::Collider::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        is_gizmo_visible = true;
        Component::show_debug_gui();
        if (ImGui::TreeNode("Surrounding Box:")) {
            (void)render::imgui::show(surrounding_box);
            ImGui::TreePop();
        }
    });
}

gearoenix::physics::collider::Collider::~Collider() = default;

bool gearoenix::physics::collider::Collider::check_intersection(const math::Aabb3<double>& box) const
{
    return surrounding_box.check_intersection(box);
}

gearoenix::math::IntersectionStatus gearoenix::physics::collider::Collider::check_intersection_status(
    const math::Aabb3<double>& box) const
{
    return surrounding_box.check_intersection_status(box);
}

void gearoenix::physics::collider::Collider::update_all_after_transform_update()
{
    core::ecs::World::get()->parallel_system<Collider>(
        [&](const auto, auto* const cld, const auto) {
            cld->update_surrounding_box();
        });
}

void gearoenix::physics::collider::Collider::draw_gizmo()
{
    surrounding_box_changed = gizmo_manager->show(surrounding_box);
    is_gizmo_visible = false;
}

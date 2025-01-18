#include "gx-phs-cld-aabb.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../gx-phs-transformation.hpp"

void gearoenix::physics::collider::Aabb3::update_surrounding_box()
{
    if (!transform->get_changed()) {
        return;
    }
    std::array<math::Vec3<double>, 8> corners;
    original_box.get_all_corners(corners);
    const auto& m = transform->get_global_matrix();
    surrounding_box.reset((m * math::Vec4(corners[0], 1.0)).xyz());
    for (std::uint32_t i = 0; i < 8; ++i) {
        surrounding_box.put_without_update((m * math::Vec4(corners[i], 1.0)).xyz());
    }
    surrounding_box.update();
}

gearoenix::physics::collider::Aabb3::Aabb3(
    std::shared_ptr<Transformation>&& transform,
    const math::Vec3<double>& upper,
    const math::Vec3<double>& lower,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Collider(std::move(transform), core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
    , original_box(upper, lower)
{
    surrounding_box = original_box;
}

gearoenix::physics::collider::Aabb3::Aabb3(
    std::shared_ptr<Transformation>&& transform,
    const math::Aabb3<double>& original_box,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Collider(std::move(transform), core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
    , original_box(original_box)
{
    surrounding_box = original_box;
}

gearoenix::physics::collider::Aabb3::~Aabb3() = default;

void gearoenix::physics::collider::Aabb3::draw_gizmo()
{
    Collider::draw_gizmo();
    if (surrounding_box_changed) {
        std::array<math::Vec3<double>, 8> corners;
        surrounding_box.get_all_corners(corners);
        const auto& im = transform->get_inverted_global_matrix();
        original_box.reset((im * math::Vec4(corners[0], 1.0)).xyz());
        for (auto i = 1; i < corners.size(); ++i) {
            original_box.put_without_update((im * math::Vec4(corners[i], 1.0)).xyz());
        }
        original_box.update();
    }
}

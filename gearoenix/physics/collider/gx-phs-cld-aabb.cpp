#include "gx-phs-cld-aabb.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"

gearoenix::physics::collider::Aabb3::Aabb3(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name, const core::ecs::entity_id_t entity_id)
    : Component(core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
    , original_box(upper, lower)
    , updated_box(original_box)
{
}

gearoenix::physics::collider::Aabb3::Aabb3(const math::Aabb3<double>& original_box, std::string&& name, const core::ecs::entity_id_t entity_id)
    : Component(core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
    , original_box(original_box)
    , updated_box(original_box)
{
}

gearoenix::physics::collider::Aabb3::~Aabb3() = default;

void gearoenix::physics::collider::Aabb3::update(const math::Mat4x4<double>& transform)
{
    std::array<math::Vec3<double>, 8> corners;
    original_box.get_all_corners(corners);
    auto tmp = transform * math::Vec4(corners[0], 1.0);
    updated_box.reset(tmp.xyz());
    for (std::uint32_t i = 0; i < 8; ++i) {
        tmp = transform * math::Vec4(corners[i], 1.0);
        updated_box.put_without_update(tmp.xyz());
    }
    updated_box.update();
}

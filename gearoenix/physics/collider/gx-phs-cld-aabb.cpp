#include "gx-phs-cld-aabb.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"

namespace {
const auto allocator = gearoenix::core::allocator::SharedArray<gearoenix::physics::collider::Aabb3, 8192>::construct();
}

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::physics::collider::Aabb3::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types(this);
    return types;
}

gearoenix::physics::collider::Aabb3::Aabb3(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name, const core::ecs::entity_id_t entity_id)
    : Component(create_this_type_index(this), std::move(name), entity_id)
    , original_box(upper, lower)
    , updated_box(original_box)
{
}

gearoenix::physics::collider::Aabb3::Aabb3(const math::Aabb3<double>& original_box, std::string&& name, const core::ecs::entity_id_t entity_id)
    : Component(create_this_type_index(this), std::move(name), entity_id)
    , original_box(original_box)
    , updated_box(original_box)
{
}

std::shared_ptr<gearoenix::physics::collider::Aabb3> gearoenix::physics::collider::Aabb3::construct(
    const math::Vec3<double>& upper,
    const math::Vec3<double>& lower,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
{
    auto self = allocator->make_shared(upper, lower, std::move(name), entity_id);
    return self;
}

std::shared_ptr<gearoenix::physics::collider::Aabb3> gearoenix::physics::collider::Aabb3::construct(
    const math::Aabb3<double>& original_box,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
{
    auto self = allocator->make_shared(original_box, std::move(name), entity_id);
    return self;
}

gearoenix::physics::collider::Aabb3::~Aabb3() = default;

void gearoenix::physics::collider::Aabb3::update(const math::Mat4x4<double>& transform)
{
    std::array<math::Vec3<double>, 8> corners;
    original_box.get_all_corners(corners);
    auto tmp = transform * math::Vec4(corners[0], 1.0);
    updated_box.reset(tmp.xyz());
    for (std::size_t i = 0; i < 8; ++i) {
        tmp = transform * math::Vec4(corners[i], 1.0);
        updated_box.put_without_update(tmp.xyz());
    }
    updated_box.update();
}

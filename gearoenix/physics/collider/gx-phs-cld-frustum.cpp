#include "gx-phs-cld-frustum.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"

namespace {
const auto frustum_allocator = gearoenix::core::allocator::SharedArray<gearoenix::physics::collider::Frustum, 16>::construct();
}

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::physics::collider::Frustum::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types(this);
    return types;
}

gearoenix::physics::collider::Frustum::Frustum(std::string&& name, const std::array<math::Vec3<double>, 8>& points,
    const core::ecs::entity_id_t entity_id)
    : Component(create_this_type_index(this), std::move(name), entity_id)
    , frustum(points)
{
    for (const auto& p : points) {
        surrounding_box.put_without_update(p);
    }
    surrounding_box.update();
}

std::shared_ptr<gearoenix::physics::collider::Frustum> gearoenix::physics::collider::Frustum::construct(
    std::string&& name, const std::array<math::Vec3<double>, 8>& points,
    const core::ecs::entity_id_t entity_id)
{
    auto self = frustum_allocator->make_shared(std::move(name), points, entity_id);
    self->set_component_self(self);
    return self;
}

void gearoenix::physics::collider::Frustum::update(const std::array<math::Vec3<double>, 8>& points)
{
    frustum = math::Frustum(points);
    surrounding_box.reset(points[0]);
    for (std::size_t i = 1; i < points.size(); ++i) {
        surrounding_box.put_without_update(points[i]);
    }
    surrounding_box.update();
}

bool gearoenix::physics::collider::Frustum::check_intersection(const math::Aabb3<double>& b) const
{
    return surrounding_box.check_intersection(b) && frustum.check_intersection_status(b) != math::IntersectionStatus::Out;
}

gearoenix::math::IntersectionStatus gearoenix::physics::collider::Frustum::check_intersection_status(const math::Aabb3<double>& b) const
{
    if (surrounding_box.check_intersection(b)) {
        return frustum.check_intersection_status(b);
    }
    return math::IntersectionStatus::Out;
}

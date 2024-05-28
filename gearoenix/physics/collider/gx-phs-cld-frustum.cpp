#include "gx-phs-cld-frustum.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"

namespace {
const auto frustum_allocator = gearoenix::core::allocator::SharedArray<gearoenix::physics::collider::Frustum, 16>::construct();
}

const boost::container::flat_set<std::type_index>& gearoenix::physics::collider::Frustum::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set<std::type_index> types { core::ecs::Component::create_this_type_index(this) };
    return types;
}

gearoenix::physics::collider::Frustum::Frustum(std::string&& name, const std::array<math::Vec3<double>, 8>& points)
    : core::ecs::Component(core::ecs::Component::create_this_type_index(this), std::move(name))
    , frustum(points)
{
    for (const auto& p : points)
        surrounding_box.put_without_update(p);
    surrounding_box.update();
}

std::shared_ptr<gearoenix::physics::collider::Frustum> gearoenix::physics::collider::Frustum::construct(
    std::string&& name, const std::array<math::Vec3<double>, 8>& points)
{
    auto self = frustum_allocator->make_shared(std::move(name), points);
    self->set_component_self(self);
    return self;
}

void gearoenix::physics::collider::Frustum::update(const std::array<math::Vec3<double>, 8>& points)
{
    frustum = math::Frustum<double>(points);
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
    if (surrounding_box.check_intersection(b))
        return frustum.check_intersection_status(b);
    return math::IntersectionStatus::Out;
}

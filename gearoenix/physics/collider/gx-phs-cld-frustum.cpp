#include "gx-phs-cld-frustum.hpp"

gearoenix::physics::collider::Frustum::Frustum(const std::array<math::Vec3<double>, 8>& points) noexcept
    : core::ecs::Component(this)
    , frustum(points)
{
    for (const auto& p : points)
        surrounding_box.put_without_update(p);
    surrounding_box.update();
}

void gearoenix::physics::collider::Frustum::update(const std::array<math::Vec3<double>, 8>& points) noexcept
{
    frustum = math::Frustum<double>(points);
    surrounding_box.reset(points[0]);
    for (std::size_t i = 1; i < points.size(); ++i) {
        surrounding_box.put_without_update(points[i]);
    }
    surrounding_box.update();
}

bool gearoenix::physics::collider::Frustum::check_intersection(const math::Aabb3<double>& b) const noexcept
{
    return surrounding_box.check_intersection(b) && frustum.check_intersection_status(b) != math::IntersectionStatus::Out;
}

gearoenix::math::IntersectionStatus gearoenix::physics::collider::Frustum::check_intersection_status(const math::Aabb3<double>& b) const noexcept
{
    if (surrounding_box.check_intersection(b))
        return frustum.check_intersection_status(b);
    return math::IntersectionStatus::Out;
}
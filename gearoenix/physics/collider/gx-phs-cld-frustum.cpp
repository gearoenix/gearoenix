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
    const auto v01 = (points[1] - points[0]).normalized();
    const auto v02 = (points[2] - points[0]).normalized();
    const auto v04 = (points[4] - points[0]).normalized();

    const auto v75 = (points[5] - points[7]).normalized();
    const auto v76 = (points[6] - points[7]).normalized();
    const auto v73 = (points[3] - points[7]).normalized();

    frustum = math::Frustum<double>({ math::Plane(points[0], v02.cross(v01)),
        math::Plane(points[0], v01.cross(v04)),
        math::Plane(points[0], v04.cross(v02)),
        math::Plane(points[7], v76.cross(v75)),
        math::Plane(points[7], v75.cross(v73)),
        math::Plane(points[7], v73.cross(v76)) });

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
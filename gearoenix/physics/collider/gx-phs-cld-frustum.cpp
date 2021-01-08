#include "gx-phs-cld-frustum.hpp"

gearoenix::physics::collider::Frustum::Frustum(const math::Vec3<double> (&points)[8]) noexcept
    : Collider(Type::Frustum)
    , frustum(points)
{
}

gearoenix::physics::collider::Frustum::~Frustum() noexcept = default;

std::optional<double> gearoenix::physics::collider::Frustum::hit(const math::Ray3&, const double) const noexcept
{
    GXLOGF("This struct does not provide implementation for this function.")
}

bool gearoenix::physics::collider::Frustum::check_intersection(const math::Aabb3& box) const noexcept
{
    if (Collider::check_intersection(box)) {
        return frustum.check_intersection_status(box) != math::IntersectionStatus::Out;
    } else {
        return false;
    }
}

gearoenix::math::IntersectionStatus gearoenix::physics::collider::Frustum::check_intersection_status(const math::Aabb3& box) const noexcept
{
    if (Collider::check_intersection_status(box) == math::IntersectionStatus::Out) {
        return math::IntersectionStatus::Out;
    }
    return frustum.check_intersection_status(box);
}

gearoenix::math::Vec3<double> gearoenix::physics::collider::Frustum::get_location() const noexcept
{
    GXLOGF("This struct does not provide implementation for this function.")
}

void gearoenix::physics::collider::Frustum::set_location(const math::Vec3<double>&) noexcept
{
    GXLOGF("This struct does not provide implementation for this function.")
}

void gearoenix::physics::collider::Frustum::local_scale(const double) noexcept
{
    GXLOGF("This struct does not provide implementation for this function.")
}

void gearoenix::physics::collider::Frustum::local_x_scale(const double) noexcept
{
    GXLOGF("This struct does not provide implementation for this function.")
}

void gearoenix::physics::collider::Frustum::update(const math::Vec3<double> (&points)[8]) noexcept
{
    const auto v01 = (points[1] - points[0]).normalized();
    const auto v02 = (points[2] - points[0]).normalized();
    const auto v04 = (points[4] - points[0]).normalized();

    const auto v75 = (points[5] - points[7]).normalized();
    const auto v76 = (points[6] - points[7]).normalized();
    const auto v73 = (points[3] - points[7]).normalized();

    frustum = math::Frustum({ math::Plane(points[0], v02.cross(v01)),
        math::Plane(points[0], v01.cross(v04)),
        math::Plane(points[0], v04.cross(v02)),
        math::Plane(points[7], v76.cross(v75)),
        math::Plane(points[7], v75.cross(v73)),
        math::Plane(points[7], v73.cross(v76)) });

    updated_box.reset(points[0]);
    for (unsigned int i = 1; i < GX_COUNT_OF(points); ++i) {
        updated_box.put_without_update(points[i]);
    }
    updated_box.update();
}

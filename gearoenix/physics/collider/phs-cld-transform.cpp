#include "phs-cld-transform.hpp"

gearoenix::physics::collider::Transform::Transform() noexcept
    : Collider(Type::Transform)
{
}

gearoenix::physics::collider::Transform::~Transform() noexcept = default;

std::optional<double> gearoenix::physics::collider::Transform::hit(const math::Ray3&, const double) const noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}

bool gearoenix::physics::collider::Transform::check_intersection(const math::Aabb3& box) const noexcept
{
#define GX_MAP_AABB_POINTS                                                            \
    math::Vec3<double> cs[8];                                                         \
    box.get_all_corners(cs);                                                          \
    math::Aabb3 proj_box((matrix * math::Vec4<double>(cs[0], 1.0)).xyz());            \
    for (unsigned int i = 1; i < GX_COUNT_OF(cs); ++i)                                \
        proj_box.put_without_update((matrix * math::Vec4<double>(cs[i], 1.0)).xyz()); \
    proj_box.update();

    GX_MAP_AABB_POINTS
    return limit.check_intersection(proj_box);
}

gearoenix::math::IntersectionStatus gearoenix::physics::collider::Transform::check_intersection_status(const math::Aabb3& box) const noexcept
{
    GX_MAP_AABB_POINTS
    return limit.check_intersection_status(proj_box);
}

const gearoenix::math::Vec3<double>& gearoenix::physics::collider::Transform::get_location() const noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}

void gearoenix::physics::collider::Transform::set_location(const math::Vec3<double>&) noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}

void gearoenix::physics::collider::Transform::local_scale(const double) noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}

void gearoenix::physics::collider::Transform::local_x_scale(const double) noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}
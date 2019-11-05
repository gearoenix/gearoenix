#include "phs-cld-frustum.hpp"
#include "../../system/sys-log.hpp"

gearoenix::physics::collider::Frustum::Frustum() noexcept
    : Collider(Type::Frustum)
    , limit(math::Vec3(1.0f), math::Vec3(-1.0f))
{
}

gearoenix::physics::collider::Frustum::~Frustum() noexcept = default;

std::optional<gearoenix::core::Real> gearoenix::physics::collider::Frustum::hit(const math::Ray3&, const core::Real) const noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}

bool gearoenix::physics::collider::Frustum::check_intersection(const math::Aabb3& box) const noexcept
{
#define GX_MAP_AABB_POINTS                        \
    math::Vec3 cs[8];                             \
    box.get_all_corners(cs);                      \
    math::Aabb3 proj_box(view_projection* cs[0]); \
    for (std::size_t i = 1; i < 8; ++i)           \
    proj_box.put(view_projection* cs[i])

    GX_MAP_AABB_POINTS;
    return limit.check_intersection(proj_box);
}

gearoenix::math::IntersectionStatus gearoenix::physics::collider::Frustum::check_intersection_status(const math::Aabb3& box) const noexcept
{
    GX_MAP_AABB_POINTS;
    return limit.check_intersection_status(proj_box);
}

const gearoenix::math::Vec3& gearoenix::physics::collider::Frustum::get_location() const noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}

void gearoenix::physics::collider::Frustum::set_location(const gearoenix::math::Vec3&) noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}

void gearoenix::physics::collider::Frustum::local_scale(const core::Real) noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}

void gearoenix::physics::collider::Frustum::local_x_scale(const core::Real) noexcept
{
    GXLOGF("This class does not provide implementation for this function.")
}

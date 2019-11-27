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
#define GX_MAP_AABB_POINTS                                   \
    math::Vec3 cs[8];                                        \
    box.get_all_corners(cs);                                 \
    math::Aabb3 proj_box;                                    \
    for (const math::Vec3& c3: cs) {                         \
        math::Vec4 c(c3, 1.0f);                              \
        c = view_projection* c;                              \
        proj_box.put_without_update(c.xyz() / c[3]);         \
    }                                                        \
    proj_box.update()

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

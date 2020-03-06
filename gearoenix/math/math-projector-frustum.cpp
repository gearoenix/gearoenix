#include "math-projector-frustum.hpp"
#include "math-aabb.hpp"
#include "math-sphere.hpp"

gearoenix::math::ProjectorFrustum::ProjectorFrustum(const Mat4x4<double>& view_projection) noexcept
    : view_projection(view_projection)
{
}

void gearoenix::math::ProjectorFrustum::set_view_projection(const Mat4x4<double>& v) noexcept
{
    view_projection = v;
}

gearoenix::math::IntersectionStatus gearoenix::math::ProjectorFrustum::check_intersection(const Sphere& s) const noexcept
{
    return check_intersection(s.get_center(), s.get_radius());
}

gearoenix::math::IntersectionStatus gearoenix::math::ProjectorFrustum::check_intersection(const Vec3<double>& position, const double radius) const noexcept
{
    return check_intersection({
        position + Vec3(radius, radius, radius),
        position + Vec3(-radius, radius, radius),
        position + Vec3(radius, -radius, radius),
        position + Vec3(-radius, -radius, radius),
        position + Vec3(radius, radius, -radius),
        position + Vec3(-radius, radius, -radius),
        position + Vec3(radius, -radius, -radius),
        position + Vec3(-radius, -radius, -radius),
    });
}

gearoenix::math::IntersectionStatus gearoenix::math::ProjectorFrustum::check_intersection(const Aabb3& aabb) const noexcept
{
    const Vec3<double>& c = aabb.get_center();
    const Vec3 r = aabb.get_diameter() * 0.5f;
    return check_intersection({
        aabb.get_upper(),
        c + Vec3(-r.x, r.y, r.z),
        c + Vec3(r.x, -r.y, r.z),
        c + Vec3(-r.x, -r.y, r.z),
        c + Vec3(r.x, r.y, -r.z),
        c + Vec3(-r.x, r.y, -r.z),
        c + Vec3(r.x, -r.y, -r.z),
        aabb.get_lower(),
    });
}

gearoenix::math::IntersectionStatus gearoenix::math::ProjectorFrustum::check_intersection(const Vec3<double> (&points)[8]) const noexcept
{
    Aabb3 aabb;
    for (const auto& p : points) {
        const auto pp = view_projection * Vec4(p, 1.0);
        aabb.put(pp.xyz() / pp.w);
    }
    // TODO it can be better the z part is redundant
    static const Aabb3 u(Vec3(1.0, 1.0, 1.0), Vec3(-1.0, -1.0, -1.0));
    return u.check_intersection_status(aabb);
}

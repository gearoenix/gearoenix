#include "math-plane.hpp"
#include "math-sphere.hpp"

gearoenix::math::Plane::Plane(const Vec3& point, const Vec3& normal) noexcept
    : normal(normal)
    , point(point)
    , d(-(normal.dot(point)))
{
}

gearoenix::math::IntersectionStatus::Type gearoenix::math::Plane::check_intersection(const Sphere& s) const noexcept
{
    const core::Real sd = s.position.dot(normal) + d;
    if (sd > s.radius)
        return IntersectionStatus::Above;
    if (sd == s.radius)
        return IntersectionStatus::Tangent;
    if (sd <= -s.radius)
        return IntersectionStatus::Under;
    return IntersectionStatus::Cut;
}
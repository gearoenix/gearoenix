#ifndef GEAROENIX_MATH_PLANE_HPP
#define GEAROENIX_MATH_PLANE_HPP
#include "math-intersection-status.hpp"
#include "math-vector-3d.hpp"

namespace gearoenix::math {
struct Sphere;
template <typename Element>
struct Plane {
    Vec3<Element> normal;
    Element d;
    Vec3<Element> point;

    Plane() = delete;

    /// Normal must be normalized.
    Plane(const Vec3<Element>& point, const Vec3<Element>& normalized_normal) noexcept
        : normal(normalized_normal)
        , d(-(normalized_normal.dot(point)))
        , point(point)
    {
    }

    Plane(const Vec3<Element>& point1, const Vec3<Element>& point2, const Vec3<Element>& point3) noexcept
        : normal((point2 - point1).cross(point3 - point1).normalized())
        , d(-(normal.dot(point1)))
        , point(point1)
    {
    }

    [[nodiscard]] Element get_distance(const Vec3<Element>& p) const noexcept
    {
        return p.dot(normal) + d;
    }

    //    [[nodiscard]] IntersectionStatus check_intersection(const Sphere& s) const noexcept;
};
}
#endif
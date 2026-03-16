#pragma once
#include "gx-math-intersection-status.hpp"
#include "gx-math-vector-4d.hpp"

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::math {
template <typename Element>
struct Sphere;

template <typename Element>
struct Plane {
    Vec4<Element> normal_d;
    Vec3<Element> point;

    Plane() = delete;

    /// Normal must be normalised.
    constexpr Plane(const Vec3<Element>& point, const Vec3<Element>& normalized_normal)
        : normal_d(normalized_normal, -(normalized_normal.dot(point)))
        , point(point)
    {
    }

    constexpr Plane(const Vec3<Element>& point1, const Vec3<Element>& point2, const Vec3<Element>& point3)
        : normal_d((point2 - point1).cross(point3 - point1).normalised(), static_cast<Element>(0))
        , point(point1)
    {
        normal_d.w = -(point.dot(normal_d.xyz()));
    }

    [[nodiscard]] constexpr Element get_distance(const Vec3<Element>& p) const { return normal_d.dot(Vec4<Element>(p, static_cast<Element>(1))); }

    void write(platform::stream::Stream& s) const
    {
        normal_d.write(s);
        point.write(s);
    }

    void read(platform::stream::Stream& s)
    {
        normal_d.read(s);
        point.read(s);
    }
};
}
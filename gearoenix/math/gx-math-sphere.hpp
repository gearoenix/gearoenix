#pragma once
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-math-intersection-status.hpp"
#include "gx-math-vector-3d.hpp"
#include <optional>

namespace gearoenix::math {
template <typename Element>
struct Ray3;

template <typename Element>
struct Sphere final {
    GX_GET_CREF_PRV(Vec3<Element>, center)
    GX_GET_VAL_PRV(Element, radius, 0.0)
    GX_GET_VAL_PRV(Element, radius2, 0.0)

public:
    Sphere() = delete;

    constexpr Sphere(const Vec3<Element>& center, const Element radius)
        : center(center)
        , radius(radius)
        , radius2(radius * radius)
    {
    }

    constexpr void set_radius(const Element r)
    {
        radius2 = r * r;
        radius = r;
    }

    constexpr void set_center(const Vec3<Element>& c) { center = c; }

    constexpr void insert(const Element r)
    {
        if (radius < r) {
            set_radius(r);
        }
    }

    [[nodiscard]] constexpr std::optional<Element> hit(const math::Ray3<Element>& r) const
    {
        const auto dir = center - r.get_origin();
        const auto dir2 = dir.square_length();
        if (dir2 <= radius2) // Ignore inside rays
            return std::nullopt;
        const auto dis = r.get_normalized_direction().dot(dir);
        if (dis <= 0.0f)
            return std::nullopt;
        const auto dis2 = dis * dis;
        const auto a2 = dir2 - dis2;
        if (radius2 <= a2)
            return std::nullopt;
        return dis - std::sqrt(radius2 - a2);
    }

    [[nodiscard]] constexpr std::optional<Element> hit(const math::Ray3<Element>& r, const Element d_min) const
    {
        if (const auto h = hit(r))
            if ((*h) < d_min)
                return h;
        return std::nullopt;
    }

    [[nodiscard]] constexpr IntersectionStatus check_intersection(const Sphere<Element>& o) const
    {
        const auto l = (center - o.center).length();
        const auto r = radius + o.radius;
        if (l > r)
            return IntersectionStatus::Out;
        if (l == r)
            return IntersectionStatus::Touch;
        Element mnr, mxr;
        if (radius > o.radius) {
            mnr = o.radius;
            mxr = radius;
        } else {
            mnr = radius;
            mxr = o.radius;
        }
        if (l + mnr >= mxr)
            return IntersectionStatus::Cut;
        return IntersectionStatus::In;
    }
};
}
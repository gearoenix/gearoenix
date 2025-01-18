#pragma once
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-math-intersection-status.hpp"
#include "gx-math-matrix-4d.hpp"
#include "gx-math-vector-3d.hpp"
#include <array>
#include <limits>
#include <optional>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace gearoenix::math {
template <typename Element>
struct Ray3;

template <typename Element>
struct Sphere;

template <typename Element>
struct Aabb3 final {
    constexpr static std::array<int, 8> counter_corners_indices = { 7, 6, 5, 4, 3, 2, 1, 0 };

    GX_GET_CREF_PRV(Vec3<Element>, upper);
    GX_GET_CREF_PRV(Vec3<Element>, lower);
    GX_GET_CREF_PRV(Vec3<Element>, diameter);
    GX_GET_CREF_PRV(Vec3<Element>, center);
    GX_GET_VAL_PRV(Element, volume, static_cast<Element>(0));

public:
    constexpr Aabb3()
        : upper(Vec3(-std::numeric_limits<Element>::max(), -std::numeric_limits<Element>::max(), -std::numeric_limits<Element>::max()))
        , lower(Vec3(std::numeric_limits<Element>::max(), std::numeric_limits<Element>::max(), std::numeric_limits<Element>::max()))
        , diameter(static_cast<Element>(0))
        , center(static_cast<Element>(0))
        , volume(static_cast<Element>(0))
    {
    }

    constexpr Aabb3(const Vec3<Element>& upper, const Vec3<Element>& lower)
        : upper(upper)
        , lower(lower)
        , diameter(upper - lower)
        , center((upper + lower) * static_cast<Element>(0.5))
        , volume(diameter.x * diameter.y * diameter.z)
    {
    }

    constexpr explicit Aabb3(const Vec3<Element>& point)
        : upper(point)
        , lower(point)
        , diameter(static_cast<Element>(0))
        , center(point)
        , volume(static_cast<Element>(0))
    {
    }

    template <typename T>
    constexpr explicit Aabb3(const Aabb3<T>& o)
        : upper(o.upper)
        , lower(o.lower)
        , diameter(o.diameter)
        , center(o.center)
        , volume(o.volume)
    {
    }

    constexpr explicit Aabb3(const Mat4x4<Element>& m)
        : diameter(m.get_diameter())
        , center(m.get_position())
        , volume(diameter.x * diameter.y * diameter.z)
    {
        const auto r = diameter * static_cast<Element>(0.5);
        upper = center + r;
        lower = center - r;
    }

    constexpr void update()
    {
        diameter = upper - lower;
        center = (upper + lower) * static_cast<Element>(0.5);
        volume = diameter.x * diameter.y * diameter.z;
    }

    constexpr void reset()
    {
        upper = Vec3(-std::numeric_limits<Element>::max(), -std::numeric_limits<Element>::max(), -std::numeric_limits<Element>::max());
        lower = Vec3(std::numeric_limits<Element>::max(), std::numeric_limits<Element>::max(), std::numeric_limits<Element>::max());
        diameter = Vec3<Element>(static_cast<Element>(0));
        center = Vec3<Element>(static_cast<Element>(0));
        volume = static_cast<Element>(0);
    }

    constexpr void reset(const Vec3<Element>& point)
    {
        upper = point;
        lower = point;
        diameter = Vec3<Element>(static_cast<Element>(0));
        center = point;
        volume = static_cast<Element>(0);
    }

    constexpr void reset(const Vec3<Element>& upper_point, const Vec3<Element>& lower_point)
    {
        upper = upper_point;
        lower = lower_point;
        diameter = upper - lower;
        center = (upper + lower) * static_cast<Element>(0.5);
        volume = diameter.x * diameter.y * diameter.z;
    }

    constexpr void put(const Vec3<Element>& point)
    {
        put_without_update(point);
        update();
    }

    constexpr void put(const Sphere<Element>& sphere)
    {
        const auto u = sphere.get_center() + sphere.get_radius();
        const auto l = sphere.get_center() - sphere.get_radius();

        upper = upper.maximum(u);
        lower = lower.minimum(l);

        update();
    }

    constexpr void put(const Aabb3& o)
    {
        put_without_update(o);
        update();
    }

    /// For better performance but keep in mind call update after your puts finished
    /// In case your not sure about it, use put, it is safer
    constexpr void put_without_update(const Vec3<Element>& point)
    {
        upper = upper.maximum(point);
        lower = lower.minimum(point);
    }

    /// For better performance but keep in mind call update after your puts finished
    /// In case your not sure about it, use put, it is safer
    constexpr void put_without_update(const Aabb3& o)
    {
        upper = upper.maximum(o.upper);
        lower = lower.minimum(o.lower);
    }

    [[nodiscard]] constexpr Mat4x4<Element> to_matrix() const
    {
        Mat4x4<Element> r;
        r.set_position(center);
        r.local_scale(diameter);
        return r;
    }

    [[nodiscard]] constexpr std::optional<Element> hit(const Ray3<Element>& ray, const Element d_min) const
    {
        const auto& ro = ray.get_origin();
        const auto& rrd = ray.get_reversed_normalized_direction();
        const auto t0 = (lower - ro) * rrd;
        const auto t1 = (upper - ro) * rrd;
        const auto t_min = t0.minimum(t1).maximum();
        const auto t_max = t0.maximum(t1).minimum();
        if (t_max >= static_cast<Element>(0) && t_min <= t_max && t_min < d_min) {
            return t_min;
        }
        return std::nullopt;
    }

    [[nodiscard]] constexpr IntersectionStatus check_intersection_status(const Aabb3& o) const
    {
        if (upper.x > o.upper.x
            && upper.y > o.upper.y
            && upper.z > o.upper.z
            && lower.x < o.lower.x
            && lower.y < o.lower.y
            && lower.z < o.lower.z) {
            return IntersectionStatus::In;
        }
        if (check_intersection(o)) {
            return IntersectionStatus::Cut;
        }
        return IntersectionStatus::Out;
    }

    [[nodiscard]] constexpr bool check_intersection(const Sphere<Element>& sphere) const
    {
        return check_intersection(Aabb3(
            sphere.get_center() + sphere.get_radius(), sphere.get_center() - sphere.get_radius()));
    }

    [[nodiscard]] constexpr bool check_intersection(const Aabb3& o, Aabb3& intersection) const
    {
        int equals = 0;
        const auto u = upper.minimum(o.upper);
        const auto l = lower.maximum(o.lower);
        for (int i = 0; i < 3; ++i) {
            const auto uv = u[i];
            const auto lv = l[i];
            if (uv == lv) {
                ++equals;
            } else if (lv > uv) {
                return false;
            }
        }
        if (equals < 3) {
            intersection = Aabb3(u, l);
            return true;
        }
        return false;
    }

    [[nodiscard]] constexpr bool check_intersection(const Aabb3& o) const
    {
        const auto max_dis = diameter + o.diameter;
        const auto dis = (center - o.center).abs() * static_cast<Element>(2);
        return max_dis.greater(dis).and_elements();
    }

    constexpr void set_center(const Vec3<Element>& c)
    {
        const auto t = c - center;
        center = c;
        upper += t;
        lower += t;
    }

    constexpr void set_diameter(const Vec3<Element>& d)
    {
        diameter = d;
        const auto hd = d * 0.5;
        upper = center + hd;
        lower = center - hd;
        volume = d.x * d.y * d.z;
    }

    constexpr void get_all_corners(std::array<Vec3<Element>, 8>& corners) const
    {
        corners[0].x = upper.x;
        corners[0].y = upper.y;
        corners[0].z = upper.z;

        corners[1].x = lower.x;
        corners[1].y = upper.y;
        corners[1].z = upper.z;

        corners[2].x = upper.x;
        corners[2].y = lower.y;
        corners[2].z = upper.z;

        corners[3].x = lower.x;
        corners[3].y = lower.y;
        corners[3].z = upper.z;

        corners[4].x = upper.x;
        corners[4].y = upper.y;
        corners[4].z = lower.z;

        corners[5].x = lower.x;
        corners[5].y = upper.y;
        corners[5].z = lower.z;

        corners[6].x = upper.x;
        corners[6].y = lower.y;
        corners[6].z = lower.z;

        corners[7].x = lower.x;
        corners[7].y = lower.y;
        corners[7].z = lower.z;
    }

    void write(platform::stream::Stream& s) const
    {
        upper.write(s);
        lower.write(s);
    }

    void read(platform::stream::Stream& s)
    {
        upper.read(s);
        lower.read(s);
        update();
    }
};
}
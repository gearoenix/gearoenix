#ifndef GEAROENIX_MATH_FRUSTUM_HPP
#define GEAROENIX_MATH_FRUSTUM_HPP
#include "gx-math-aabb.hpp"
#include "gx-math-plane.hpp"
#include <array>

namespace gearoenix::math {
template <typename Element>
struct Frustum final {
    /// Inward planes
    std::array<Plane<Element>, 6> planes;

    Frustum() = delete;

    /// Points must be as following
    ///   0----------1
    ///   |\        /|
    ///   | 4------5 |
    ///   | | near | |
    ///   | 6------7 |
    ///   |/        \|
    ///   2----------3
    constexpr explicit Frustum(const std::array<Vec3<Element>, 8>& points) noexcept
        : planes {
            Plane(points[0], points[2], points[1]),
            Plane(points[0], points[1], points[4]),
            Plane(points[0], points[4], points[2]),
            Plane(points[7], points[6], points[5]),
            Plane(points[7], points[5], points[3]),
            Plane(points[7], points[3], points[6]),
        }
    {
    }

    constexpr explicit Frustum(const std::array<Plane<Element>, 6>& planes) noexcept
        : planes(planes)
    {
    }

    /// This is not exact but rather fast, necessarily informative and conservatively includes objects
    // [[nodiscard]] IntersectionStatus check_intersection(const Sphere& s) const noexcept;

    /// This is not exact but rather fast, necessarily informative and conservatively includes objects
    /// It is not a completely correct, sometime it includes non-colliding objects
    [[nodiscard]] constexpr IntersectionStatus check_intersection_status(const Aabb3<Element>& aabb) const noexcept
    {
        auto status = IntersectionStatus::In;
        std::array<Vec3<Element>, 8> points;
        aabb.get_all_corners(points);
        for (const auto& plane : planes) {
            int outs_count = 0;
            for (const auto& point : points) {
                if (plane.get_distance(point) < static_cast<Element>(0))
                    ++outs_count;
            }
            if (outs_count == 8)
                return IntersectionStatus::Out;
            if (outs_count != 0)
                status = IntersectionStatus::Cut;
        }
        return status;
    }
};
}
#endif
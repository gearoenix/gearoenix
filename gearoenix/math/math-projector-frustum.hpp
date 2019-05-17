#ifndef GEAROENIX_MATH_PROJECTOR_FRUSTUM_HPP
#define GEAROENIX_MATH_PROJECTOR_FRUSTUM_HPP
#include "math-intersection-status.hpp"
#include "math-matrix.hpp"
namespace gearoenix {
namespace math {
    struct Aabb3;
    struct Sphere;
    struct ProjectorFrustum {
        Mat4x4 view_projection;

        ProjectorFrustum() = delete;
        explicit ProjectorFrustum(const Mat4x4& view_projection) noexcept;
        IntersectionStatus::Type check_intersection(const Sphere& s) const noexcept;
        IntersectionStatus::Type check_intersection(const Aabb3& aabb) const noexcept;
        IntersectionStatus::Type check_intersection(const Vec3 (&points)[8]) const noexcept;
    };
}
}
#endif
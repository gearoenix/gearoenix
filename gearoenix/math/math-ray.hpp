#ifndef GEAROENIX_MATH_RAY_HPP
#define GEAROENIX_MATH_RAY_HPP
#include "../core/cr-static.hpp"
#include "math-vector.hpp"
#include <utility>
namespace gearoenix::math {
struct Ray3 {
    GX_GET_REF_PRV(Vec3, origin)
    GX_GET_REF_PRV(Vec3, normalized_direction)
    GX_GET_REF_PRV(Vec3, reversed_normalized_direction)
public:
    Ray3(const Vec3& o, const Vec3& nd) noexcept
        : origin(o)
        , normalized_direction(nd)
        , reversed_normalized_direction(Vec3(1.0f) / nd)
    {
    }
    [[nodiscard]] Vec3 get_point(const core::Real d) const noexcept { return normalized_direction * d + origin; }
};
}
#endif
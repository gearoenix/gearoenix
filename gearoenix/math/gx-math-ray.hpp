#ifndef GEAROENIX_MATH_RAY_HPP
#define GEAROENIX_MATH_RAY_HPP
#include "../core/gx-cr-static.hpp"
#include "gx-math-vector-3d.hpp"
#include <utility>
namespace gearoenix::math {
struct Ray3 {
    GX_GET_REF_PRV(Vec3<double>, origin)
    GX_GET_REF_PRV(Vec3<double>, normalized_direction)
    GX_GET_REF_PRV(Vec3<double>, reversed_normalized_direction)
public:
    Ray3(const Vec3<double>& o, const Vec3<double>& nd) noexcept
        : origin(o)
        , normalized_direction(nd)
        , reversed_normalized_direction(Vec3<double>(1.0) / nd)
    {
    }

    [[nodiscard]] Vec3<double> get_point(const double d) const noexcept
    {
        return (normalized_direction * d) + origin;
    }
};
}
#endif
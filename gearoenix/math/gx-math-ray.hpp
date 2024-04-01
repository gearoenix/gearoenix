#ifndef GEAROENIX_MATH_RAY_HPP
#define GEAROENIX_MATH_RAY_HPP
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-math-vector-3d.hpp"
#include <utility>
namespace gearoenix::math {
template <typename Element>
struct Ray3 {
    GX_GET_REF_PRV(Vec3<Element>, origin);
    GX_GET_REF_PRV(Vec3<Element>, normalized_direction);
    GX_GET_REF_PRV(Vec3<Element>, reversed_normalized_direction);

public:
    Ray3(const Vec3<Element>& o, const Vec3<Element>& nd)
        : origin(o)
        , normalized_direction(nd)
        , reversed_normalized_direction(Vec3<Element>(1.0) / nd)
    {
    }

    [[nodiscard]] Vec3<Element> get_point(const Element d) const
    {
        return (normalized_direction * d) + origin;
    }
};
}
#endif
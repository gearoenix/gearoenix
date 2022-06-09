#ifndef GEAROENIX_MATH_MATRIX_2D_HPP
#define GEAROENIX_MATH_MATRIX_2D_HPP
#include "gx-math-vector-2d.hpp"

namespace gearoenix::math {
/// Be careful this Matrix implementation in column major way.
template <typename Element>
struct Mat2x2 {
    Element e0;
    Element e1;
    Element e2;
    Element e3;

    constexpr explicit Mat2x2(const Element diameter = static_cast<Element>(0)) noexcept
        : e0(diameter)
        , e1(static_cast<Element>(0))
        , e2(static_cast<Element>(0))
        , e3(diameter)
    {
    }

    constexpr Mat2x2(const Element e0, const Element e1, const Element e2, const Element e3) noexcept
        : e0(e0)
        , e1(e1)
        , e2(e2)
        , e3(e3)
    {
    }

    constexpr bool invert(const Element zero_tolerance = static_cast<Element>(0.0001)) noexcept
    {
        static_assert(std::numeric_limits<Element>::is_signed,
            "This functionality is only available for signed element types.");
        const auto det = e0 * e3 - e1 * e2;
        if (det < -zero_tolerance && det > zero_tolerance)
            return false;
        std::swap(e0, e3);
        e1 = -e1;
        e2 = -e2;
        const auto m = static_cast<Element>(1) / det;
        e0 *= m;
        e1 *= m;
        e2 *= m;
        e3 *= m;
        return true;
    }

    constexpr Vec2<Element> operator*(const Vec2<Element>& v) const noexcept
    {
        return Vec2(e0 * v.x + e1 * v.y, e2 * v.x + e3 * v.y);
    }
};
}
#endif

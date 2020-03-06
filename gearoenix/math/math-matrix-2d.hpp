#ifndef GEAROENIX_MATH_MATRIX_2D_HPP
#define GEAROENIX_MATH_MATRIX_2D_HPP
#include "math-vector-2d.hpp"

namespace gearoenix::math {
/// Be careful this Matrix implementation in column major way.
template <typename Element>
struct Mat2x2 {
    Element e0;
    Element e1;
    Element e2;
    Element e3;

    constexpr Mat2x2() noexcept
        : e0(static_cast<Element>(0))
        , e1(static_cast<Element>(0))
        , e2(static_cast<Element>(0))
        , e3(static_cast<Element>(0))
    {
    }

    constexpr explicit Mat2x2(const Element diameter) noexcept
        : e0(diameter)
        , e1(static_cast<Element>(0))
        , e2(static_cast<Element>(0))
        , e3(diameter)
    {
    }

    Mat2x2(const Element e0, const Element e1, const Element e2, const Element e3) noexcept
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

    template <typename T, typename R>
    constexpr Vec2<R> operator*(const Vec2<T>& v) const noexcept
    {
        return Vec2(
            static_cast<R>(e0) * static_cast<R>(v.x) + static_cast<R>(e1) * static_cast<R>(v.y),
            static_cast<R>(e2) * static_cast<R>(v.x) + static_cast<R>(e3) * static_cast<R>(v.y));
    }
};
}
#endif

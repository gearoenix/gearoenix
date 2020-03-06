#ifndef GEAROENIX_MATH_VECTOR_2D_HPP
#define GEAROENIX_MATH_VECTOR_2D_HPP

#include "../system/stream/sys-stm-stream.hpp"
#include "math-numeric.hpp"
#include <cmath>
#include <limits>
#include <optional>
#include <ostream>

namespace gearoenix::math {
template <typename Element>
struct Vec2 {
    Element x = static_cast<Element>(0);
    Element y = static_cast<Element>(0);

    constexpr Vec2() noexcept
        : x(static_cast<Element>(0))
        , y(static_cast<Element>(0))
    {
    }

    template <typename T, typename R>
    constexpr Vec2(const T x, const R y) noexcept
        : x(static_cast<Element>(x))
        , y(static_cast<Element>(y))
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(R, Element)
    }

    template <typename T>
    constexpr explicit Vec2(const Vec2<T>& o) noexcept
        : x(static_cast<Element>(o.x))
        , y(static_cast<Element>(o.y))
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
    }

    template <typename T>
    constexpr explicit Vec2(const T e) noexcept
        : x(static_cast<Element>(e))
        , y(static_cast<Element>(e))
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> operator+(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return Vec2(x + static_cast<Element>(o.x), y + static_cast<Element>(o.y));
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> operator-(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return Vec2(x - static_cast<Element>(o.x), y - static_cast<Element>(o.y));
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> operator*(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return Vec2(x * static_cast<Element>(o.x), y * static_cast<Element>(o.y));
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> operator/(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return Vec2(x / static_cast<Element>(o.x), y / static_cast<Element>(o.y));
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> operator+(const T e) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return Vec2(x + static_cast<Element>(e), y + static_cast<Element>(e));
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> operator-(const T e) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return Vec2(x - static_cast<Element>(e), y - static_cast<Element>(e));
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> operator*(const T e) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return Vec2(x * static_cast<Element>(e), y * static_cast<Element>(e));
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> operator/(const T e) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return Vec2(x / static_cast<Element>(e), y / static_cast<Element>(e));
    }

    [[nodiscard]] constexpr typename std::enable_if<std::numeric_limits<Element>::is_signed, Vec2<Element>>::type operator-() const noexcept
    {
        return Vec2(-x, -y);
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element>& operator=(const Vec2<T>& o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x = static_cast<Element>(o.x);
        y = static_cast<Element>(o.y);
        return *this;
    }

    template <typename T>
    constexpr void operator+=(const Vec2<T>& o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x += static_cast<Element>(o.x);
        y += static_cast<Element>(o.y);
    }

    template <typename T>
    constexpr void operator-=(const Vec2<T>& o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x -= static_cast<Element>(o.x);
        y -= static_cast<Element>(o.y);
    }

    template <typename T>
    constexpr void operator*=(const Vec2<T>& o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x *= static_cast<Element>(o.x);
        y *= static_cast<Element>(o.y);
    }

    template <typename T>
    constexpr void operator/=(const Vec2<T>& o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x /= static_cast<Element>(o.x);
        y /= static_cast<Element>(o.y);
    }

    template <typename T>
    constexpr void operator+=(const T e) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x += static_cast<Element>(e);
        y += static_cast<Element>(e);
    }

    template <typename T>
    constexpr void operator-=(const T e) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x -= static_cast<Element>(e);
        y -= static_cast<Element>(e);
    }

    template <typename T>
    constexpr void operator*=(const T e) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x *= static_cast<Element>(e);
        y *= static_cast<Element>(e);
    }

    template <typename T>
    constexpr void operator/=(const T e) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const auto m = static_cast<Element>(1) / static_cast<Element>(e);
        *this /= m;
    }

    template <typename T>
    [[nodiscard]] constexpr const typename std::enable_if<std::numeric_limits<T>::is_integer, Element>::type&
    operator[](const T i) const noexcept
    {
        switch (i) {
        case static_cast<T>(0):
            return x;
        case static_cast<T>(1):
            return y;
        default:
            return *reinterpret_cast<Element>(nullptr);
        }
    }

    template <typename T>
    [[nodiscard]] constexpr typename std::enable_if<std::numeric_limits<T>::is_integer, Element>::type&
    operator[](const T i) noexcept
    {
        switch (i) {
        case static_cast<T>(0):
            return x;
        case static_cast<T>(1):
            return y;
        default:
            return *reinterpret_cast<Element>(nullptr);
        }
    }

    /// Y is the most valuable part then X
    template <typename T>
    [[nodiscard]] constexpr bool operator<(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return y < static_cast<Element>(o.y) || (y == static_cast<Element>(o.y) && x < static_cast<Element>(o.x));
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator==(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return x == static_cast<Element>(o.x) && y == static_cast<Element>(o.y);
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator>(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return y > static_cast<Element>(o.y) || (y == static_cast<Element>(o.y) && x < static_cast<Element>(o.x));
    }

    template <typename T>
    [[nodiscard]] constexpr Element dot(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return x * static_cast<Element>(o.x) + y * static_cast<Element>(o.y);
    }

    template <typename T>
    [[nodiscard]] constexpr Element cross(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return x * o.y - y * o.x;
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> max(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const auto ox = static_cast<Element>(o.x);
        const auto oy = static_cast<Element>(o.y);
        return Vec2(GX_MAX(x, ox), GX_MAX(y, oy));
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<Element> min(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const auto ox = static_cast<Element>(o.x);
        const auto oy = static_cast<Element>(o.y);
        return Vec2(GX_MIN(x, ox), GX_MIN(y, oy));
    }

    [[nodiscard]] constexpr Element length() const noexcept
    {
        return std::sqrt(square_length());
    }

    [[nodiscard]] constexpr Element square_length() const noexcept
    {
        return dot(*this);
    }

    template <typename T>
    [[nodiscard]] constexpr Element distance(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return std::sqrt(square_distance(*this));
    }

    template <typename T>
    [[nodiscard]] constexpr Element square_distance(const Vec2<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return (*this - o).square_length();
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<T> normalized() const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(Element, T)
        const auto e = static_cast<T>(1) / static_cast<T>(length());
        return Vec2(static_cast<T>(x) * e, static_cast<T>(y) * e);
    }

    constexpr void normalize() noexcept
    {
        *this /= length();
    }

    void read(system::stream::Stream* const f) noexcept
    {
        f->read(x);
        f->read(y);
    }

    [[nodiscard]] static constexpr std::optional<Vec2<Element>> intersect(
        const Vec2<Element>& s11,
        const Vec2<Element>& s12,
        const Vec2<Element>& s21,
        const Vec2<Element>& s22) noexcept;

    [[nodiscard]] static constexpr Vec2 hammersley(const std::uint32_t i, const std::uint32_t n) noexcept
    {
        return Vec2(static_cast<Element>(i) / static_cast<Element>(n), static_cast<Element>(Numeric::radical_inverse_vdc(i)));
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2<Element>& v) noexcept
    {
        os << R"("Vec2": { "x": ")" << v.x << R"(", "y": ")" << v.y << "\"}";
        return os;
    }
};
}

#include "math-matrix-2d.hpp"

template <typename Element>
constexpr std::optional<gearoenix::math::Vec2<Element>> gearoenix::math::Vec2<Element>::intersect(
    const Vec2<Element>& s11,
    const Vec2<Element>& s12,
    const Vec2<Element>& s21,
    const Vec2<Element>& s22) noexcept
{
    const auto max1 = s11.max(s12);
    const auto min1 = s11.min(s12);
    const auto max2 = s21.max(s22);
    const auto min2 = s21.min(s22);
    if (min1.x > max2.x || min2.x > max1.x || min1.y > max2.y || min2.y > max1.y)
        return std::nullopt;
    // s11 + s0 * v0 == s21 + s1 * v1
    // s21 - s11 = s0 * v0 - s1 * v1
    // d = | v0 v1 | * s
    const auto v0 = s12 - s11;
    const auto v1 = s22 - s21;
    const Mat2x2 m(v0.x, -v1.x, v0.y, -v1.y);
    if (!m.invert())
        return std::nullopt;
    const auto d = s21 - s11;
    const auto s = m * d;
    if (s.x < static_cast<Element>(0) || s.x > static_cast<Element>(1) || s.y < static_cast<Element>(0) || s.y > static_cast<Element>(1))
        return std::nullopt;
    return v0 * s.x + s11;
}

#endif
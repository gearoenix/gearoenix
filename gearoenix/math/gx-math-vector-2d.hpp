#ifndef GEAROENIX_MATH_VECTOR_2D_HPP
#define GEAROENIX_MATH_VECTOR_2D_HPP

#include "../platform/gx-plt-log.hpp"
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "gx-math-numeric.hpp"
#include <cmath>
#include <limits>
#include <optional>
#include <ostream>
#include <type_traits>

namespace gearoenix::math {
template <typename Element>
struct Vec2 {
    Element x = static_cast<Element>(0);
    Element y = static_cast<Element>(0);

    constexpr explicit Vec2(const Element e = static_cast<Element>(0)) noexcept
        : x(e)
        , y(e)
    {
    }

    constexpr Vec2(const Element x, const Element y) noexcept
        : x(x)
        , y(y)
    {
    }

    constexpr Vec2(const Vec2<Element>& o) noexcept = default;

    template <typename T>
    constexpr explicit Vec2(const Vec2<T>& o) noexcept
        : x(static_cast<Element>(o.x))
        , y(static_cast<Element>(o.y))
    {
        static_assert(!std::is_same_v<Element, T>, "Only different type can be constructed by this constructor.");
    }

    [[nodiscard]] constexpr Vec2<Element> operator+(const Vec2<Element>& o) const noexcept
    {
        return Vec2(x + o.x, y + o.y);
    }

    [[nodiscard]] constexpr Vec2<Element> operator-(const Vec2<Element>& o) const noexcept
    {
        return Vec2(x - o.x, y - o.y);
    }

    [[nodiscard]] constexpr Vec2<Element> operator*(const Vec2<Element>& o) const noexcept
    {
        return Vec2(x * o.x, y * o.y);
    }

    [[nodiscard]] constexpr Vec2<Element> operator/(const Vec2<Element>& o) const noexcept
    {
        return Vec2(x / o.x, y / o.y);
    }

    [[nodiscard]] constexpr Vec2<Element> operator+(const Element e) const noexcept
    {
        return Vec2(x + e, y + e);
    }

    [[nodiscard]] constexpr Vec2<Element> operator-(const Element e) const noexcept
    {
        return Vec2(x - e, y - e);
    }

    [[nodiscard]] constexpr Vec2<Element> operator*(const Element e) const noexcept
    {
        return Vec2(x * e, y * e);
    }

    [[nodiscard]] constexpr Vec2<Element> operator/(const Element e) const noexcept
    {
        const auto m = static_cast<Element>(1) / e;
        return Vec2(x * m, y * m);
    }

    [[nodiscard]] constexpr Vec2<Element> operator-() const noexcept
    {
        Numeric::check_signable<Element>();
        return Vec2(-x, -y);
    }

    constexpr Vec2<Element>& operator=(const Vec2<Element>& o) noexcept
    {
        x = o.x;
        y = o.y;
        return *this;
    }

    constexpr void operator+=(const Vec2<Element>& o) noexcept
    {
        x += o.x;
        y += o.y;
    }

    constexpr void operator-=(const Vec2<Element>& o) noexcept
    {
        Numeric::check_signable<Element>();
        x -= o.x;
        y -= o.y;
    }

    constexpr void operator*=(const Vec2<Element>& o) noexcept
    {
        x *= o.x;
        y *= o.y;
    }

    constexpr void operator/=(const Vec2<Element>& o) noexcept
    {
        x /= o.x;
        y /= o.y;
    }

    constexpr void operator+=(const Element e) noexcept
    {
        x += e;
        y += e;
    }

    constexpr void operator-=(const Element e) noexcept
    {
        x -= e;
        y -= e;
    }

    constexpr void operator*=(const Element e) noexcept
    {
        x *= e;
        y *= e;
    }

    constexpr void operator/=(const Element e) noexcept
    {
        const auto m = static_cast<Element>(1) / e;
        *this *= m;
    }

    template <typename T>
    [[nodiscard]] typename std::enable_if<std::numeric_limits<T>::is_integer, Element>::type
    operator[](const T i) const noexcept
    {
        switch (i) {
        case static_cast<T>(0):
            return x;
        case static_cast<T>(1):
            return y;
        default:
            GX_LOG_F("Out of bound index: " << i);
        }
    }

    template <typename T>
    [[nodiscard]] typename std::enable_if<std::numeric_limits<T>::is_integer, Element>::type&
    operator[](const T i) noexcept
    {
        switch (i) {
        case static_cast<T>(0):
            return x;
        case static_cast<T>(1):
            return y;
        default:
            GX_LOG_F("Out of bound index: " << i);
        }
    }

    /// Y is the most valuable part then X
    [[nodiscard]] constexpr bool operator<(const Vec2<Element>& o) const noexcept
    {
        return y < o.y || (y == o.y && x < o.x);
    }

    /// Y is the most valuable part then X
    [[nodiscard]] constexpr bool operator<=(const Vec2<Element>& o) const noexcept
    {
        return y < o.y || (y == o.y && x <= o.x);
    }

    [[nodiscard]] constexpr bool operator==(const Vec2<Element>& o) const noexcept
    {
        return x == o.x && y == o.y;
    }

    [[nodiscard]] constexpr bool operator>(const Vec2<Element>& o) const noexcept
    {
        return y > o.y || (y == o.y && x > o.x);
    }

    [[nodiscard]] constexpr bool operator>=(const Vec2<Element>& o) const noexcept
    {
        return y > o.y || (y == o.y && x >= o.x);
    }

    [[nodiscard]] constexpr Element dot(const Vec2<Element>& o) const noexcept
    {
        return x * o.x + y * o.y;
    }

    [[nodiscard]] constexpr Element cross(const Vec2<Element>& o) const noexcept
    {
        return x * o.y - y * o.x;
    }

    [[nodiscard]] constexpr Vec2<Element> maximum(const Vec2<Element>& o) const noexcept
    {
        return Vec2(x > o.x ? x : o.x, y > o.y ? y : o.y);
    }

    [[nodiscard]] constexpr Vec2<Element> safe_maximum(const Vec2<Element>& o) const noexcept
    {
        return Vec2(Numeric::safe_maximum(x, o.x), Numeric::safe_maximum(y, o.y));
    }

    [[nodiscard]] constexpr Vec2<Element> minimum(const Vec2<Element>& o) const noexcept
    {
        return Vec2(x < o.x ? x : o.x, y < o.y ? y : o.y);
    }

    [[nodiscard]] constexpr Vec2<Element> safe_minimum(const Vec2<Element>& o) const noexcept
    {
        return Vec2(Numeric::safe_minimum(x, o.x), Numeric::safe_minimum(y, o.y));
    }

    [[nodiscard]] constexpr Element length() const noexcept
    {
        return static_cast<Element>(std::sqrt(square_length()));
    }

    [[nodiscard]] constexpr Element square_length() const noexcept
    {
        return dot(*this);
    }

    [[nodiscard]] constexpr Element distance(const Vec2<Element>& o) const noexcept
    {
        return static_cast<Element>(std::sqrt(square_distance(o)));
    }

    [[nodiscard]] constexpr Element square_distance(const Vec2<Element>& o) const noexcept
    {
        return (*this - o).square_length();
    }

    [[nodiscard]] constexpr Vec2<Element> normalized() const noexcept
    {
        return *this / length();
    }

    constexpr void normalize() noexcept
    {
        *this /= length();
    }

    void read(platform::stream::Stream& f) noexcept
    {
        x = static_cast<Element>(f.read<float>());
        y = static_cast<Element>(f.read<float>());
    }

    [[nodiscard]] static constexpr std::optional<Vec2<Element>> intersect(
        const Vec2<Element>& s11,
        const Vec2<Element>& s12,
        const Vec2<Element>& s21,
        const Vec2<Element>& s22) noexcept;

    [[nodiscard]] static constexpr Vec2 hammersley(const std::uint32_t i, const std::uint32_t n) noexcept
    {
        return Vec2(static_cast<Element>(i) / static_cast<Element>(n), Numeric::radical_inverse_vdc(i));
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2<Element>& v) noexcept
    {
        os << R"("Vec2": { "x": ")" << v.x << R"(", "y": ")" << v.y << "\"}";
        return os;
    }
};
}

#include "gx-math-matrix-2d.hpp"

template <typename Element>
constexpr std::optional<gearoenix::math::Vec2<Element>> gearoenix::math::Vec2<Element>::intersect(
    const Vec2<Element>& s11,
    const Vec2<Element>& s12,
    const Vec2<Element>& s21,
    const Vec2<Element>& s22) noexcept
{
    const auto max1 = s11.maximum(s12);
    const auto min1 = s11.minimum(s12);
    const auto max2 = s21.maximum(s22);
    const auto min2 = s21.minimum(s22);
    if (min1.x > max2.x || min2.x > max1.x || min1.y > max2.y || min2.y > max1.y)
        return std::nullopt;
    // s11 + s0 * v0 == s21 + s1 * v1
    // s21 - s11 = s0 * v0 - s1 * v1
    // d = | v0 v1 | * s
    const auto v0 = s12 - s11;
    const auto v1 = s22 - s21;
    Mat2x2 m(v0.x, -v1.x, v0.y, -v1.y);
    if (!m.invert())
        return std::nullopt;
    const auto d = s21 - s11;
    const auto s = m * d;
    if (s.x < static_cast<Element>(0) || s.x > static_cast<Element>(1) || s.y < static_cast<Element>(0) || s.y > static_cast<Element>(1))
        return std::nullopt;
    return v0 * s.x + s11;
}

#endif
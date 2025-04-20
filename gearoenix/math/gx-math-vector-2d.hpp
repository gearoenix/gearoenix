#pragma once
#include "../platform/stream/gx-plt-stm-stream.hpp"
#include "gx-math-numeric.hpp"
#include <cmath>
#include <optional>
#include <ostream>
#include <type_traits>

namespace gearoenix::math {
template <typename Element>
struct Vec2 final {
    Element x = static_cast<Element>(0);
    Element y = static_cast<Element>(0);

    constexpr explicit Vec2()
        : x(static_cast<Element>(0))
        , y(static_cast<Element>(0))
    {
    }

    constexpr explicit Vec2(const Element e)
        : x(e)
        , y(e)
    {
    }

    constexpr Vec2(const Element x, const Element y)
        : x(x)
        , y(y)
    {
    }

    constexpr Vec2(const Vec2& o) = default;

    template <typename T>
    constexpr explicit Vec2(const Vec2<T>& o)
        : x(static_cast<Element>(o.x))
        , y(static_cast<Element>(o.y))
    {
        static_assert(!std::is_same_v<Element, T>, "Only different type can be constructed by this constructor.");
    }

    [[nodiscard]] constexpr Vec2 operator+(const Vec2& o) const
    {
        return { x + o.x, y + o.y };
    }

    [[nodiscard]] constexpr Vec2 operator-(const Vec2& o) const
    {
        return { x - o.x, y - o.y };
    }

    [[nodiscard]] constexpr Vec2 operator*(const Vec2& o) const
    {
        return { x * o.x, y * o.y };
    }

    [[nodiscard]] constexpr Vec2 operator/(const Vec2& o) const
    {
        return { x / o.x, y / o.y };
    }

    [[nodiscard]] constexpr Vec2 operator+(const Element e) const
    {
        return { x + e, y + e };
    }

    [[nodiscard]] constexpr Vec2 operator-(const Element e) const
    {
        return { x - e, y - e };
    }

    [[nodiscard]] constexpr Vec2 operator*(const Element e) const
    {
        return { x * e, y * e };
    }

    [[nodiscard]] constexpr Vec2 operator/(const Element e) const
    {
        if constexpr (std::is_floating_point_v<Element>) {
            const auto m = static_cast<Element>(1) / e;
            return { x * m, y * m };
        } else {
            return { x / e, y / e };
        }
    }

    [[nodiscard]] constexpr Vec2 operator-() const
    {
        Numeric::check_signable<Element>();
        return { -x, -y };
    }

    constexpr Vec2& operator=(const Vec2& o) = default;

    constexpr void operator+=(const Vec2& o)
    {
        x += o.x;
        y += o.y;
    }

    constexpr void operator-=(const Vec2& o)
    {
        Numeric::check_signable<Element>();
        x -= o.x;
        y -= o.y;
    }

    constexpr void operator*=(const Vec2& o)
    {
        x *= o.x;
        y *= o.y;
    }

    constexpr void operator/=(const Vec2& o)
    {
        x /= o.x;
        y /= o.y;
    }

    constexpr void operator+=(const Element e)
    {
        x += e;
        y += e;
    }

    constexpr void operator-=(const Element e)
    {
        x -= e;
        y -= e;
    }

    constexpr void operator*=(const Element e)
    {
        x *= e;
        y *= e;
    }

    constexpr void operator/=(const Element e)
    {
        if constexpr (std::is_floating_point_v<Element>) {
            const auto m = static_cast<Element>(1) / e;
            *this *= m;
        } else {
            x /= e;
            y /= e;
        }
    }

    template <typename T>
    [[nodiscard]] constexpr Element operator[](const T i) const
    {
        return data()[i];
    }

    template <typename T>
    [[nodiscard]] constexpr Element& operator[](const T i)
    {
        return data()[i];
    }

    /// Y is the most valuable part then X
    [[nodiscard]] constexpr bool operator<(const Vec2& o) const
    {
        return y < o.y || (y == o.y && x < o.x);
    }

    /// Y is the most valuable part then X
    [[nodiscard]] constexpr bool operator<=(const Vec2& o) const
    {
        return y < o.y || (y == o.y && x <= o.x);
    }

    [[nodiscard]] constexpr bool operator==(const Vec2& o) const
    {
        return x == o.x && y == o.y;
    }

    [[nodiscard]] constexpr bool operator==(const Element v) const
    {
        return x == v && y == v;
    }

    [[nodiscard]] constexpr bool operator>(const Vec2& o) const
    {
        return y > o.y || (y == o.y && x > o.x);
    }

    [[nodiscard]] constexpr bool operator>=(const Vec2& o) const
    {
        return y > o.y || (y == o.y && x >= o.x);
    }

    [[nodiscard]] constexpr bool equal(const Element v, const Element tolerance = Numeric::epsilon<Element>) const
    {
        if (*this == v) {
            return true;
        }
        const auto b = (*this - v).abs().less(tolerance);
        return b.x && b.y;
    }

    [[nodiscard]] constexpr bool equal(const Vec2& o, const Element tolerance = Numeric::epsilon<Element>) const
    {
        if (o == *this) {
            return true;
        }
        const auto b = (*this - o).abs().less(tolerance);
        return b.x && b.y;
    }

    [[nodiscard]] constexpr Vec2 abs() const
    {
        return { std::abs(x), std::abs(y) };
    }

    [[nodiscard]] constexpr Vec2<bool> less(const Element v) const
    {
        return { x < v, y < v };
    }

    [[nodiscard]] constexpr Vec2<bool> less(const Vec2& v) const
    {
        return { x < v.x, y < v.y };
    }

    [[nodiscard]] constexpr bool and_all() const
    {
        return x && y;
    }

    [[nodiscard]] constexpr Element dot(const Vec2& o) const
    {
        return x * o.x + y * o.y;
    }

    [[nodiscard]] constexpr Element cross(const Vec2& o) const
    {
        return x * o.y - y * o.x;
    }

    [[nodiscard]] constexpr Vec2 maximum(const Vec2& o) const
    {
        return { x > o.x ? x : o.x, y > o.y ? y : o.y };
    }

    [[nodiscard]] constexpr Vec2 safe_maximum(const Vec2& o) const
    {
        return { Numeric::safe_maximum(x, o.x), Numeric::safe_maximum(y, o.y) };
    }

    [[nodiscard]] constexpr Vec2 minimum(const Vec2& o) const
    {
        return { x < o.x ? x : o.x, y < o.y ? y : o.y };
    }

    [[nodiscard]] constexpr Vec2 safe_minimum(const Vec2& o) const
    {
        return { Numeric::safe_minimum(x, o.x), Numeric::safe_minimum(y, o.y) };
    }

    [[nodiscard]] constexpr Element length() const
    {
        return static_cast<Element>(std::sqrt(square_length()));
    }

    [[nodiscard]] constexpr Element square_length() const
    {
        return dot(*this);
    }

    [[nodiscard]] constexpr Element distance(const Vec2& o) const
    {
        return static_cast<Element>(std::sqrt(square_distance(o)));
    }

    [[nodiscard]] constexpr Element square_distance(const Vec2& o) const
    {
        return (*this - o).square_length();
    }

    [[nodiscard]] constexpr Vec2 normalized() const
    {
        return *this / length();
    }

    [[nodiscard]] constexpr const Element* data() const
    {
        return &x;
    }

    [[nodiscard]] constexpr Element* data()
    {
        return &x;
    }

    constexpr void normalize()
    {
        *this /= length();
    }

    void read(platform::stream::Stream& f)
    {
        x = f.read<Element>();
        y = f.read<Element>();
    }

    void write(platform::stream::Stream& s) const
    {
        s.write_fail_debug(x);
        s.write_fail_debug(y);
    }

    [[nodiscard]] constexpr static std::optional<Vec2> intersect(
        const Vec2& s11,
        const Vec2& s12,
        const Vec2& s21,
        const Vec2& s22);

    [[nodiscard]] constexpr static Vec2 hammersley(const std::uint32_t i, const std::uint32_t n)
    {
        return { static_cast<Element>(i) / static_cast<Element>(n), Numeric::radical_inverse_vdc(i) };
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2& v)
    {
        os << R"("Vec2": { "x": ")" << v.x << R"(", "y": ")" << v.y << "\"}";
        return os;
    }
};
}

#include "gx-math-matrix-2d.hpp"

template <typename Element>
constexpr std::optional<gearoenix::math::Vec2<Element>> gearoenix::math::Vec2<Element>::intersect(
    const Vec2& s11,
    const Vec2& s12,
    const Vec2& s21,
    const Vec2& s22)
{
    const auto max1 = s11.maximum(s12);
    const auto min1 = s11.minimum(s12);
    const auto max2 = s21.maximum(s22);
    const auto min2 = s21.minimum(s22);
    if (min1.x > max2.x || min2.x > max1.x || min1.y > max2.y || min2.y > max1.y) {
        return std::nullopt;
    }
    // s11 + s0 * v0 == s21 + s1 * v1
    // s21 - s11 = s0 * v0 - s1 * v1
    // d = | v0 v1 | * s
    const auto v0 = s12 - s11;
    const auto v1 = s22 - s21;
    Mat2x2 m(v0.x, -v1.x, v0.y, -v1.y);
    if (!m.invert()) {
        return std::nullopt;
    }
    const auto d = s21 - s11;
    const auto s = m * d;
    if (s.x < static_cast<Element>(0) || s.x > static_cast<Element>(1) || s.y < static_cast<Element>(0) || s.y > static_cast<Element>(1)) {
        return std::nullopt;
    }
    return v0 * s.x + s11;
}
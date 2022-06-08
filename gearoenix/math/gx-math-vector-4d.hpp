#ifndef GEAROENIX_MATH_VECTOR_4D_HPP
#define GEAROENIX_MATH_VECTOR_4D_HPP
#include "gx-math-vector-3d.hpp"

namespace gearoenix::math {
template <typename Element>
struct Vec4 {
    Element x, y, z, w;

    constexpr Vec4(const Element x, const Element y, const Element z, const Element w) noexcept
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    constexpr explicit Vec4(const Vec2<Element>& v, const Element z = static_cast<Element>(0),
        const Element w = static_cast<Element>(0)) noexcept
        : x(v.x)
        , y(v.y)
        , z(z)
        , w(w)
    {
    }

    constexpr explicit Vec4(const Vec3<Element>& v, const Element w = static_cast<Element>(0)) noexcept
        : x(v.x)
        , y(v.y)
        , z(v.z)
        , w(w)
    {
    }

    constexpr explicit Vec4(const Element e = static_cast<Element>(0)) noexcept
        : x(e)
        , y(e)
        , z(e)
        , w(e)
    {
    }

    constexpr Vec4(const Vec4<Element>& o) noexcept = default;

    template <typename T>
    constexpr explicit Vec4(const Vec4<T>& o) noexcept
        : x(static_cast<Element>(o.x))
        , y(static_cast<Element>(o.y))
        , z(static_cast<Element>(o.z))
        , w(static_cast<Element>(o.w))
    {
        static_assert(!std::is_same_v<Element, T>, "Only different type can be used by this constructor.");
    }

    template <typename T>
    constexpr explicit Vec4(const Vec2<T>& v1, const Vec2<T>& v2) noexcept
        : x(static_cast<Element>(v1.x))
        , y(static_cast<Element>(v1.y))
        , z(static_cast<Element>(v2.x))
        , w(static_cast<Element>(v2.y))
    {
        static_assert(!std::is_same_v<Element, T>, "Only different type can be used by this constructor.");
    }

    [[nodiscard]] constexpr Vec3<Element> xyz() const noexcept
    {
        return Vec3<Element>(x, y, z);
    }

    constexpr void xyz(const Vec3<Element>& v) noexcept
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    template <typename T>
    [[nodiscard]] Element operator[](const T i) const noexcept
    {
        static_assert(std::numeric_limits<T>::is_integer, "Only integer type is acceptable for indexing.");
        switch (i) {
        case static_cast<T>(0):
            return x;
        case static_cast<T>(1):
            return y;
        case static_cast<T>(2):
            return z;
        case static_cast<T>(3):
            return w;
        default:
            GX_LOG_F("Out of bound index: " << i);
        }
    }

    template <typename T>
    [[nodiscard]] Element& operator[](const T i) noexcept
    {
        static_assert(std::numeric_limits<T>::is_integer, "Only integer type is acceptable for indexing.");
        switch (i) {
        case static_cast<T>(0):
            return x;
        case static_cast<T>(1):
            return y;
        case static_cast<T>(2):
            return z;
        case static_cast<T>(3):
            return w;
        default:
            GX_LOG_F("Out of bound index: " << i);
        }
    }

    [[nodiscard]] constexpr bool operator<(const Vec4<Element>& o) const noexcept
    {
        return w < o.w || (w == o.w && (z < o.z || (z == o.z && (y < o.y || (y == o.y && x < o.x)))));
    }

    [[nodiscard]] constexpr bool operator<=(const Vec4<Element>& o) const noexcept
    {
        return w < o.w || (w == o.w && (z < o.z || (z == o.z && (y < o.y || (y == o.y && x <= o.x)))));
    }

    [[nodiscard]] constexpr bool operator>(const Vec4<Element>& o) const noexcept
    {
        return w > o.w || (w == o.w && (z > o.z || (z == o.z && (y > o.y || (y == o.y && x > o.x)))));
    }

    [[nodiscard]] constexpr bool operator>=(const Vec4<Element>& o) const noexcept
    {
        return w > o.w || (w == o.w && (z > o.z || (z == o.z && (y > o.y || (y == o.y && x >= o.x)))));
    }

    [[nodiscard]] constexpr bool operator==(const Vec4<Element>& o) const noexcept
    {
        return w == o.w && z == o.z && y == o.y && x == o.x;
    }

    [[nodiscard]] constexpr Vec4<Element> operator*(const Element o) const noexcept
    {
        return Vec4<Element>(x * o, y * o, z * o, w * o);
    }

    [[nodiscard]] constexpr Vec4<Element> operator*(const Vec4<Element>& o) const noexcept
    {
        return Vec4<Element>(x * o.x, y * o.y, z * o.z, w * o.w);
    }

    [[nodiscard]] constexpr Vec4<Element> operator/(const Element o) const noexcept
    {
        const auto m = static_cast<Element>(1) / o;
        return *this * m;
    }

    constexpr void operator*=(const Element o) noexcept
    {
        x *= o;
        y *= o;
        z *= o;
        w *= o;
    }

    constexpr void operator/=(const Element o) noexcept
    {
        const auto m = static_cast<Element>(1) / o;
        *this *= m;
    }

    [[nodiscard]] constexpr Vec4<Element> operator-(const Vec4<Element>& o) const noexcept
    {
        return Vec4<Element>(x - o.x, y - o.y, z - o.z, w - o.w);
    }

    [[nodiscard]] constexpr Element length() const noexcept
    {
        return static_cast<Element>(std::sqrt(square_length()));
    }

    [[nodiscard]] constexpr Element square_length() const noexcept
    {
        return dot(*this);
    }

    [[nodiscard]] constexpr Element dot(const Vec4<Element>& o) const noexcept
    {
        return x * o.x + y * o.y + z * o.z + w * o.w;
    }

    [[nodiscard]] constexpr Vec4<Element> cross(const Vec4<Element>& o) const noexcept
    {
        return Vec4<Element>((y * o.z) - (z * o.y), (z * o.x) - (x * o.z), (x * o.y) - (y * o.x), w * o.w);
    }

    [[nodiscard]] constexpr Vec4<Element> normalized() const noexcept
    {
        return *this / length();
    }

    constexpr void normalize() const noexcept
    {
        *this /= length();
    }

    void read(platform::stream::Stream& f) noexcept
    {
        x = static_cast<Element>(f.read<float>());
        y = static_cast<Element>(f.read<float>());
        z = static_cast<Element>(f.read<float>());
        w = static_cast<Element>(f.read<float>());
    }

    [[nodiscard]] const Element* data() const noexcept
    {
        return reinterpret_cast<const Element*>(this);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec4<Element>& v) noexcept
    {
        os << R"("Vec4": { "x": ")" << v.x << R"(", "y": ")" << v.y << R"(", "z": ")" << v.z << R"(", "w": ")" << v.w << R"(" })";
        return os;
    }
};
}
#endif
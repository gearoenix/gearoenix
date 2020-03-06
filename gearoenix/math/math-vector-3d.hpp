#ifndef GEAROENIX_MATH_VECTOR_3D_HPP
#define GEAROENIX_MATH_VECTOR_3D_HPP
#include "math-vector-2d.hpp"

namespace gearoenix::math {
template <typename Element>
struct Vec3 {
    Element x, y, z;

    static constexpr Vec3<Element> X = Vec3<Element>(1, 0, 0);
    static constexpr Vec3<Element> Y = Vec3<Element>(0, 1, 0);
    static constexpr Vec3<Element> Z = Vec3<Element>(0, 0, 1);

    constexpr Vec3() noexcept
        : x(static_cast<Element>(0))
        , y(static_cast<Element>(0))
        , z(static_cast<Element>(0))
    {
    }

    template <typename T>
    constexpr explicit Vec3(const Vec2<T>& o, const Element e = static_cast<Element>(0)) noexcept
        : x(static_cast<Element>(o.x))
        , y(static_cast<Element>(o.y))
        , z(e)
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
    }

    constexpr Vec3(const Element x, const Element y, const Element z) noexcept
        : x(x)
        , y(y)
        , z(z)
    {
    }

    template <typename T, typename R, typename S>
    constexpr Vec3(const T x, const R y, const S z) noexcept
        : x(static_cast<Element>(x))
        , y(static_cast<Element>(y))
        , z(static_cast<Element>(z))
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(R, Element)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(S, Element)
    }

    template <typename T>
    constexpr explicit Vec3(const Vec3<T>& o) noexcept
        : x(static_cast<Element>(o.x))
        , y(static_cast<Element>(o.y))
        , z(static_cast<Element>(o.z))
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
    }

    template <typename T>
    constexpr explicit Vec3(const T e) noexcept
        : x(static_cast<Element>(e))
        , y(static_cast<Element>(e))
        , z(static_cast<Element>(e))
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
    }

    template <typename T>
    constexpr Vec3<T> operator-() const noexcept
    {
        GX_SIGNEDNESS_CHECK(T)
        GX_SIGNEDNESS_CHECK(Element)
        return Vec3<T>(-x, -y, -z);
    }

    template <typename T, typename R>
    constexpr Vec3<R> operator+(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(Element, R)
        return Vec3<R>(
            x + static_cast<Element>(o.x),
            y + static_cast<Element>(o.y),
            z + static_cast<Element>(o.z));
    }

    template <typename T, typename R>
    constexpr Vec3<R> operator-(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_CHECK(T)
        GX_SIGNEDNESS_CHECK(R)
        GX_SIGNEDNESS_CHECK(Element)
        return Vec3<R>(
            x - static_cast<Element>(o.x),
            y - static_cast<Element>(o.y),
            z - static_cast<Element>(o.z));
    }

    template <typename T, typename R>
    constexpr Vec3<R> operator*(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(Element, R)
        return Vec3<R>(
            x * static_cast<Element>(o.x),
            y * static_cast<Element>(o.y),
            z * static_cast<Element>(o.z));
    }

    template <typename T, typename R>
    constexpr Vec3<R> operator/(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(Element, R)
        return Vec3<R>(
            x / static_cast<Element>(o.x),
            y / static_cast<Element>(o.y),
            z / static_cast<Element>(o.z));
    }

    template <typename T, typename R>
    constexpr Vec3<R> operator+(const T o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(Element, R)
        return Vec3<R>(
            x + static_cast<Element>(o),
            y + static_cast<Element>(o),
            z + static_cast<Element>(o));
    }

    template <typename T, typename R>
    constexpr Vec3<R> operator-(const T o) const noexcept
    {
        GX_SIGNEDNESS_CHECK(T)
        GX_SIGNEDNESS_CHECK(R)
        GX_SIGNEDNESS_CHECK(Element)
        return Vec3<R>(
            x - static_cast<Element>(o),
            y - static_cast<Element>(o),
            z - static_cast<Element>(o));
    }

    template <typename T, typename R>
    constexpr Vec3<R> operator*(const T o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(Element, R)
        return Vec3<R>(
            x * static_cast<Element>(o),
            y * static_cast<Element>(o),
            z * static_cast<Element>(o));
    }

    template <typename T, typename R>
    constexpr Vec3<R> operator/(const T o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(Element, R)
        const auto m = static_cast<Element>(1) / static_cast<Element>(o);
        return Vec3<R>(x * m, y * m, z * m);
    }

    template <typename T>
    constexpr Vec3<Element>& operator=(const Vec3<T>& o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x = static_cast<Element>(o.x);
        y = static_cast<Element>(o.y);
        z = static_cast<Element>(o.z);
        return *this;
    }

    template <typename T>
    constexpr void operator+=(const Vec3<T>& o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x += static_cast<Element>(o.x);
        y += static_cast<Element>(o.y);
        z += static_cast<Element>(o.z);
    }

    template <typename T>
    constexpr void operator-=(const Vec3<T>& o) noexcept
    {
        GX_SIGNEDNESS_CHECK(Element)
        GX_SIGNEDNESS_CHECK(T)
        x -= static_cast<Element>(o.x);
        y -= static_cast<Element>(o.y);
        z -= static_cast<Element>(o.z);
    }

    template <typename T>
    constexpr void operator*=(const Vec3<T>& o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x *= static_cast<Element>(o.x);
        y *= static_cast<Element>(o.y);
        z *= static_cast<Element>(o.z);
    }

    template <typename T>
    constexpr void operator/=(const Vec3<T>& o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        x /= static_cast<Element>(o.x);
        y /= static_cast<Element>(o.y);
        z /= static_cast<Element>(o.z);
    }

    template <typename T>
    constexpr void operator+=(const T o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const auto m = static_cast<Element>(o);
        x += m;
        y += m;
        z += m;
    }

    template <typename T>
    constexpr void operator-=(const T o) noexcept
    {
        GX_SIGNEDNESS_CHECK(Element)
        GX_SIGNEDNESS_CHECK(T)
        const auto m = -static_cast<Element>(o);
        x += m;
        y += m;
        z += m;
    }

    template <typename T>
    constexpr void operator*=(const T o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const auto m = static_cast<Element>(o);
        x *= m;
        y *= m;
        z *= m;
    }

    template <typename T>
    constexpr void operator/=(const T o) noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const auto m = static_cast<Element>(1) / static_cast<Element>(o);
        x *= m;
        y *= m;
        z *= m;
    }

    template <typename T>
    [[nodiscard]] constexpr const Element& operator[](const T i) const noexcept
    {
        static_assert(std::numeric_limits<T>::is_integer, "Index type must be an integer kind.");
        switch (i) {
        case static_cast<T>(0):
            return x;
        case static_cast<T>(1):
            return y;
        case static_cast<T>(2):
            return z;
        default:
            return *reinterpret_cast<Element>(nullptr);
        }
    }

    template <typename T>
    [[nodiscard]] constexpr Element& operator[](const T i) noexcept
    {
        static_assert(std::numeric_limits<T>::is_integer, "Index type must be an integer kind.");
        switch (i) {
        case static_cast<T>(0):
            return x;
        case static_cast<T>(1):
            return y;
        case static_cast<T>(2):
            return z;
        default:
            return *reinterpret_cast<Element>(nullptr);
        }
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator<(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const Vec3<Element> ov(o);
        return z < ov.z || (z == ov.z && (y < ov.y || (y == ov.y && z < ov.z)));
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator<=(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const Vec3<Element> ov(o);
        return z < ov.z || (z == ov.z && (y < ov.y || (y == ov.y && z <= ov.z)));
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator==(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const Vec3<Element> ov(o);
        return z == ov.z && y == ov.y && z == ov.z;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator>(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const Vec3<Element> ov(o);
        return z > ov.z || (z == ov.z && (y > ov.y || (y == ov.y && z > ov.z)));
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator>=(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const Vec3<Element> ov(o);
        return z > ov.z || (z == ov.z && (y > ov.y || (y == ov.y && z >= ov.z)));
    }

    template <typename T>
    [[nodiscard]] constexpr Vec2<T> xy() const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(Element, T)
        return Vec2<T>(x, y);
    }

    [[nodiscard]] constexpr const Element* data() const noexcept
    {
        reinterpret_cast<const Element*>(this);
    }

    [[nodiscard]] constexpr Element* data() noexcept
    {
        reinterpret_cast<Element*>(this);
    }

    [[nodiscard]] constexpr Element dot(const Vec3<Element>& o) const noexcept
    {
        return x * o.x + y * o.y + z * o.z;
    }

    template <typename T>
    [[nodiscard]] constexpr Element dot(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        const Vec3<Element> ov(o);
        return x * ov.x + y * ov.y + z * ov.z;
    }

    [[nodiscard]] constexpr Element square_length() const noexcept
    {
        return dot(*this);
    }

    [[nodiscard]] constexpr Element length() const noexcept
    {
        return static_cast<Element>(std::sqrt(square_length()));
    }

    [[nodiscard]] constexpr Element abs_length() const noexcept
    {
        return std::abs(x) + std::abs(y) + std::abs(z);
    }

    [[nodiscard]] constexpr Element square_distance(const Vec3<Element>& o) const noexcept
    {
        return (*this - o).square_length();
    }

    template <typename T>
    [[nodiscard]] constexpr Element square_distance(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return (*this - Vec3<Element>(o)).square_length();
    }

    [[nodiscard]] constexpr Element distance(const Vec3<Element>& o) const noexcept
    {
        return (*this - o).length();
    }

    template <typename T>
    [[nodiscard]] constexpr Element distance(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return (*this - Vec3<Element>(o)).length();
    }

    [[nodiscard]] constexpr Element abs_distance(const Vec3<Element>& o) const noexcept
    {
        return (*this - o).abs_length();
    }

    template <typename T>
    [[nodiscard]] constexpr Element abs_distance(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, Element)
        return (*this - Vec3<Element>(o)).abs_length();
    }

    template <typename T>
    [[nodiscard]] constexpr Vec3<T> abs() const noexcept
    {
        return Vec3<T>(std::abs(x), std::abs(y), std::abs(z));
    }

    [[nodiscard]] constexpr Vec3<Element> cross(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
    }

    template <typename T, typename R>
    [[nodiscard]] constexpr Vec3<R> cross(const Vec3<T>& o) const noexcept
    {
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(Element, R)
        GX_SIGNEDNESS_COMPATIBILITY_CHECK(T, R)
        return Vec3<R>(*this).cross(Vec3<R>(o));
    }

    [[nodiscard]] constexpr Vec3<Element> minimum(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(GX_MIN(x, o.x), GX_MIN(y, o.y), GX_MIN(z, o.z));
    }

    [[nodiscard]] constexpr Vec3<Element> maximum(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(GX_MAX(x, o.x), GX_MAX(y, o.y), GX_MAX(z, o.z));
    }

    [[nodiscard]] constexpr Element minimum() const noexcept
    {
        const auto e = GX_MIN(x, y);
        return GX_MIN(e, z);
    }

    [[nodiscard]] constexpr Element maximum() const noexcept
    {
        const auto e = GX_MAX(x, y);
        return GX_MAX(e, z);
    }

    [[nodiscard]] constexpr Vec3<Element> normalized() const noexcept
    {
        return *this / length();
    }

    constexpr void normalize() const noexcept
    {
        *this /= length();
    }

    [[nodiscard]] static constexpr Vec3<Element> importance_sample_ggx(
        const Vec2<Element>& xi, const Vec3<Element>& n, const Element roughness,
        const Element tolerance = static_cast<Element>(0.001)) noexcept
    {
        const auto one = static_cast<Element>(1);
        const auto two = static_cast<Element>(2);
        const auto roughness_p_2 = roughness * roughness;
        const auto roughness_p_4 = roughness_p_2 * roughness_p_2;
        const auto phi = two * static_cast<Element>(GX_PI) * xi.x;
        const auto cos_phi = static_cast<Element>(std::cos(phi));
        const auto cos_phi_p_2 = cos_phi * cos_phi;
        const auto sin_phi_p_2 = one - cos_phi_p_2;
        const auto sin_phi = static_cast<Element>(std::sqrt(sin_phi_p_2));
        const auto cos_theta = static_cast<Element>(std::sqrt((one - xi.y) / (one + (roughness_p_4 - one) * xi.y)));
        const auto cos_theta_p_2 = cos_theta * cos_theta;
        const auto sin_theta = static_cast<Element>(std::sqrt(one - cos_theta_p_2));
        const Vec3<Element> h(cos_phi * sin_theta, sin_phi * sin_theta, cos_theta);
        const Vec3<Element> up = std::abs(n.z) < (one - tolerance) ? Vec3(0.0f, 0.0f, 1.0f) : Vec3(1.0f, 0.0f, 0.0f);
        const Vec3<Element> tangent = up.cross(n).normalized();
        const Vec3<Element> bitangent = n.cross(tangent);
        const Vec3<Element> sample_vec = tangent * h.x + bitangent * h.y + n * h.z;
        return sample_vec.normalized();
    }

    void read(system::stream::Stream* f) noexcept
    {
        f->read(x);
        f->read(y);
        f->read(z);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec3<Element>& v) noexcept
    {
        os << R"("Vec3": { "x": ")" << v.x << R"(", "y": ")" << v.y << R"(", "z": ")" << v.z << R"(" })";
        return os;
    }
};
}
#endif
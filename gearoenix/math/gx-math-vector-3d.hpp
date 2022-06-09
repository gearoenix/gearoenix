#ifndef GEAROENIX_MATH_VECTOR_3D_HPP
#define GEAROENIX_MATH_VECTOR_3D_HPP

#include "gx-math-vector-2d.hpp"

namespace gearoenix::math {
template <typename Element>
struct Vec3 final {
    Element x, y, z;

    constexpr Vec3() noexcept
        : x(static_cast<Element>(0))
        , y(static_cast<Element>(0))
        , z(static_cast<Element>(0))
    {
    }

    constexpr explicit Vec3(const Element e) noexcept
        : x(e)
        , y(e)
        , z(e)
    {
    }

    constexpr explicit Vec3(const Vec2<Element>& o, const Element e = static_cast<Element>(0)) noexcept
        : x(o.x)
        , y(o.y)
        , z(e)
    {
    }

    constexpr Vec3(const Element x, const Element y, const Element z) noexcept
        : x(x)
        , y(y)
        , z(z)
    {
    }

    constexpr Vec3(const Vec3<Element>& o) noexcept = default;

    template <typename T>
    constexpr explicit Vec3(const Vec3<T>& o) noexcept
        : x(static_cast<Element>(o.x))
        , y(static_cast<Element>(o.y))
        , z(static_cast<Element>(o.z))
    {
        static_assert(!std::is_same_v<Element, T>, "Only different type can be used by this constructor.");
    }

    constexpr Vec3<Element> operator-() const noexcept
    {
        return Vec3<Element>(-x, -y, -z);
    }

    constexpr Vec3<Element> operator+(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(x + o.x, y + o.y, z + o.z);
    }

    constexpr Vec3<Element> operator-(const Vec3<Element>& o) const noexcept
    {
        Numeric::check_signable<Element>();
        return Vec3<Element>(x - o.x, y - o.y, z - o.z);
    }

    constexpr Vec3<Element> operator*(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(x * o.x, y * o.y, z * o.z);
    }

    constexpr Vec3<Element> operator/(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(x / o.x, y / o.y, z / o.z);
    }

    constexpr Vec3<Element> operator+(const Element o) const noexcept
    {
        return Vec3<Element>(x + o, y + o, z + o);
    }

    constexpr Vec3<Element> operator-(const Element o) const noexcept
    {
        Numeric::check_signable<Element>();
        return Vec3<Element>(x - o, y - o, z - o);
    }

    constexpr Vec3<Element> operator*(const Element o) const noexcept
    {
        return Vec3<Element>(x * o, y * o, z * o);
    }

    constexpr Vec3<Element> operator/(const Element o) const noexcept
    {
        const auto m = static_cast<Element>(1) / o;
        return Vec3<Element>(x * m, y * m, z * m);
    }

    constexpr Vec3<Element>& operator=(const Vec3<Element>& o) noexcept
    {
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
    }

    constexpr void operator+=(const Vec3<Element>& o) noexcept
    {
        x += o.x;
        y += o.y;
        z += o.z;
    }

    constexpr void operator-=(const Vec3<Element>& o) noexcept
    {
        Numeric::check_signable<Element>();
        x -= o.x;
        y -= o.y;
        z -= o.z;
    }

    constexpr void operator*=(const Vec3<Element>& o) noexcept
    {
        x *= o.x;
        y *= o.y;
        z *= o.z;
    }

    constexpr void operator/=(const Vec3<Element>& o) noexcept
    {
        x /= o.x;
        y /= o.y;
        z /= o.z;
    }

    constexpr void operator+=(const Element o) noexcept
    {
        x += o;
        y += o;
        z += o;
    }

    constexpr void operator-=(const Element o) noexcept
    {
        Numeric::check_signable<Element>();
        x -= o;
        y -= o;
        z -= o;
    }

    constexpr void operator*=(const Element o) noexcept
    {
        x *= o;
        y *= o;
        z *= o;
    }

    constexpr void operator/=(const Element o) noexcept
    {
        const auto m = static_cast<Element>(1) / o;
        *this *= m;
    }

    template <typename T>
    [[nodiscard]] Element operator[](const T i) const noexcept
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
            GX_LOG_F("Out of range index. " << i);
        }
    }

    template <typename T>
    [[nodiscard]] Element& operator[](const T i) noexcept
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
            GX_LOG_F("Out of range index. " << i);
        }
    }

    [[nodiscard]] constexpr bool operator<(const Vec3<Element>& o) const noexcept
    {
        return z < o.z || (z == o.z && (y < o.y || (y == o.y && z < o.z)));
    }

    [[nodiscard]] constexpr bool operator<=(const Vec3<Element>& o) const noexcept
    {
        return z < o.z || (z == o.z && (y < o.y || (y == o.y && z <= o.z)));
    }

    [[nodiscard]] constexpr bool operator==(const Vec3<Element>& o) const noexcept
    {
        return z == o.z && y == o.y && z == o.z;
    }

    [[nodiscard]] constexpr bool operator>(const Vec3<Element>& o) const noexcept
    {
        return z > o.z || (z == o.z && (y > o.y || (y == o.y && z > o.z)));
    }

    [[nodiscard]] constexpr bool operator>=(const Vec3<Element>& o) const noexcept
    {
        return z > o.z || (z == o.z && (y > o.y || (y == o.y && z >= o.z)));
    }

    [[nodiscard]] constexpr Vec2<Element> xy() const noexcept
    {
        return Vec2<Element>(x, y);
    }

    [[nodiscard]] constexpr const Element* data() const noexcept
    {
        return &x;
    }

    [[nodiscard]] constexpr Element* data() noexcept
    {
        reinterpret_cast<Element*>(this);
    }

    [[nodiscard]] constexpr Element dot(const Vec3<Element>& o) const noexcept
    {
        return x * o.x + y * o.y + z * o.z;
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

    [[nodiscard]] constexpr Element distance(const Vec3<Element>& o) const noexcept
    {
        return (*this - o).length();
    }

    [[nodiscard]] constexpr Element abs_distance(const Vec3<Element>& o) const noexcept
    {
        return (*this - o).abs_length();
    }

    /// \note Only on unit(normalised) vectors will give expected result.
    [[nodiscard]] constexpr Vec3 normalized_perpendicular() const noexcept
    {
        math::Vec3 result(static_cast<Element>(1), static_cast<Element>(0), static_cast<Element>(0));
        auto d = dot(result);
        const auto zero = d - 1;
        if (GX_IS_ZERO(zero)) {
            result.x = static_cast<Element>(0);
            result.y = static_cast<Element>(1);
            d = dot(result);
        }
        result -= *this * d;
        result.normalize();
        return result;
    }

    /// \note Only on (this) unit(normalised) vectors will give expected result.
    [[nodiscard]] constexpr Vec3 normalized_perpendicular(const Vec3& o) const noexcept
    {
        return o - (*this * dot(o));
    }

    [[nodiscard]] constexpr Vec3<Element> abs() const noexcept
    {
        return Vec3<Element>(std::abs(x), std::abs(y), std::abs(z));
    }

    [[nodiscard]] constexpr Vec3<Element> cross(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
    }

    [[nodiscard]] constexpr Vec3<Element> safe_minimum(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(Numeric::safe_minimum(x, o.x), Numeric::safe_minimum(y, o.y), Numeric::safe_minimum(z, o.z));
    }

    [[nodiscard]] constexpr Vec3<Element> minimum(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(
            x < o.x ? x : o.x,
            y < o.y ? y : o.y,
            z < o.z ? z : o.z);
    }

    [[nodiscard]] constexpr Vec3<Element> safe_maximum(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(Numeric::safe_maximum(x, o.x), Numeric::safe_maximum(y, o.y), Numeric::safe_maximum(z, o.z));
    }

    [[nodiscard]] constexpr Vec3<Element> maximum(const Vec3<Element>& o) const noexcept
    {
        return Vec3<Element>(
            x > o.x ? x : o.x,
            y > o.y ? y : o.y,
            z > o.z ? z : o.z);
    }

    [[nodiscard]] constexpr Element safe_minimum() const noexcept
    {
        return Numeric::safe_minimum(Numeric::safe_minimum(x, y), z);
    }

    [[nodiscard]] constexpr Element minimum() const noexcept
    {
        const auto m = x < y ? x : y;
        return m < z ? m : z;
    }

    [[nodiscard]] constexpr Element safe_maximum() const noexcept
    {
        return Numeric::safe_maximum(Numeric::safe_maximum(x, y), z);
    }

    [[nodiscard]] constexpr Element maximum() const noexcept
    {
        const auto m = x > y ? x : y;
        return m > z ? m : z;
    }

    [[nodiscard]] constexpr Vec3<Element> normalised() const noexcept
    {
        return *this / length();
    }

    constexpr void normalise() noexcept
    {
        *this /= length();
    }

    constexpr void clamp(const Element lower, const Element upper) noexcept
    {
        if (x < lower)
            x = lower;
        else if (x > upper)
            x = upper;
        if (y < lower)
            y = lower;
        else if (y > upper)
            y = upper;
        if (z < lower)
            z = lower;
        else if (z > upper)
            z = upper;
    }

    [[nodiscard]] constexpr Vec3 atan() const noexcept
    {
        return Vec3(
            static_cast<Element>(std::atan(static_cast<double>(x))),
            static_cast<Element>(std::atan(static_cast<double>(y))),
            static_cast<Element>(std::atan(static_cast<double>(z))));
    }

    [[nodiscard]] constexpr Vec3 sign() const noexcept
    {
        return Vec3(
            std::signbit(x) ? static_cast<Element>(-1) : static_cast<Element>(1),
            std::signbit(y) ? static_cast<Element>(-1) : static_cast<Element>(1),
            std::signbit(z) ? static_cast<Element>(-1) : static_cast<Element>(1));
    }

    /// the called vector must be normalised
    [[nodiscard]] constexpr Vec3 reflect(const Vec3& o) const noexcept
    {
        return o + (*this * (dot(o) * static_cast<Element>(-2)));
    }

    [[nodiscard]] constexpr Vec3<bool> less(const Element v) const noexcept
    {
        return Vec3<bool>(x < v, y < v, z < v);
    }

    [[nodiscard]] constexpr Vec3<bool> less(const Vec3& o) const noexcept
    {
        return Vec3<bool>(x < o.x, y < o.y, z < o.z);
    }

    [[nodiscard]] constexpr Vec3<bool> greater(const Element v) const noexcept
    {
        return Vec3<bool>(x > v, y > v, z > v);
    }

    [[nodiscard]] constexpr Vec3<bool> greater(const Vec3& o) const noexcept
    {
        return Vec3<bool>(x > o.x, y > o.y, z > o.z);
    }

    [[nodiscard]] constexpr bool and_elements() const noexcept
    {
        return x && y && z;
    }

    [[nodiscard]] constexpr bool or_elements() const noexcept
    {
        return x || y || z;
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
        const Vec3<Element> tangent = up.cross(n).normalised();
        const Vec3<Element> bitangent = n.cross(tangent);
        const Vec3<Element> sample_vec = tangent * h.x + bitangent * h.y + n * h.z;
        return sample_vec.normalised();
    }

    void read(platform::stream::Stream& s) noexcept
    {
        x = static_cast<Element>(s.read<float>());
        y = static_cast<Element>(s.read<float>());
        z = static_cast<Element>(s.read<float>());
    }

    void write(platform::stream::Stream& s) const noexcept
    {
        s.write_fail_debug(static_cast<float>(x));
        s.write_fail_debug(static_cast<float>(y));
        s.write_fail_debug(static_cast<float>(z));
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec3<Element>& v) noexcept
    {
        os << R"("Vec3": { "x": ")" << v.x << R"(", "y": ")" << v.y << R"(", "z": ")" << v.z << R"(" })";
        return os;
    }
};

template <typename Element>
constexpr Vec3<Element> Origin3D(
    static_cast<Element>(0),
    static_cast<Element>(0),
    static_cast<Element>(0));

template <typename Element>
constexpr Vec3<Element> X3D(
    static_cast<Element>(1),
    static_cast<Element>(0),
    static_cast<Element>(0));

template <typename Element>
constexpr Vec3<Element> Y3D(
    static_cast<Element>(0),
    static_cast<Element>(1),
    static_cast<Element>(0));

template <typename Element>
constexpr Vec3<Element> Z3D(
    static_cast<Element>(0),
    static_cast<Element>(0),
    static_cast<Element>(1));
}
#endif
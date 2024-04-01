#ifndef GEAROENIX_MATH_QUATERNION_HPP
#define GEAROENIX_MATH_QUATERNION_HPP
#include "../core/gx-cr-types.hpp"
#include "gx-math-matrix-4d.hpp"
#include "gx-math-numeric.hpp"
#include <ostream>

namespace gearoenix::math {
template <typename Element>
struct Quat final {
    Element x, y, z, w;

    constexpr explicit Quat(const Element e = static_cast<Element>(0))
        : x(e)
        , y(e)
        , z(e)
        , w(e)
    {
    }

    constexpr Quat(const Element x, const Element y, const Element z, const Element w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    template <typename T>
    constexpr explicit Quat(const Quat<T>& o)
        : x(static_cast<Element>(o.x))
        , y(static_cast<Element>(o.y))
        , z(static_cast<Element>(o.z))
        , w(static_cast<Element>(o.w))
    {
    }

    constexpr void normalise()
    {
        const auto inv_length = (w > static_cast<Element>(0) ? static_cast<Element>(1) : static_cast<Element>(-1)) / std::sqrt(x * x + y * y + z * z + w * w);
        x *= inv_length;
        y *= inv_length;
        z *= inv_length;
        w *= inv_length;
    }

    [[nodiscard]] constexpr Quat normalised() const
    {
        Quat q = *this;
        q.normalise();
        return q;
    }

    constexpr Mat4x4<Element> to_mat() const
    {
        const Element xx = x * x;
        const Element xy = x * y;
        const Element xz = x * z;
        const Element xw = x * w;
        const Element yy = y * y;
        const Element yz = y * z;
        const Element yw = y * w;
        const Element zz = z * z;
        const Element zw = z * w;
        Mat4x4<Element> m;
        m.data[0][0] = static_cast<Element>(1) - static_cast<Element>(2) * (yy + zz);
        m.data[0][1] = static_cast<Element>(2) * (xy + zw);
        m.data[0][2] = static_cast<Element>(2) * (xz - yw);
        m.data[1][0] = static_cast<Element>(2) * (xy - zw);
        m.data[1][1] = static_cast<Element>(1) - static_cast<Element>(2) * (xx + zz);
        m.data[1][2] = static_cast<Element>(2) * (yz + xw);
        m.data[2][0] = static_cast<Element>(2) * (xz + yw);
        m.data[2][1] = static_cast<Element>(2) * (yz - xw);
        m.data[2][2] = static_cast<Element>(1) - static_cast<Element>(2) * (xx + yy);
        return m;
    }

    [[nodiscard]] constexpr bool safe_equal(const Quat& o) const
    {
        const auto nt = normalised();
        const auto no = o.normalised();
        return Numeric::equal(nt.x, no.x) && Numeric::equal(nt.y, no.y) && Numeric::equal(nt.z, no.z) && Numeric::equal(nt.w, no.w);
    }

    [[nodiscard]] constexpr Quat operator*(const Element v) const
    {
        return Quat(x * v, y * v, z * v, w * v);
    }

    [[nodiscard]] constexpr Quat operator+(const Quat& o) const
    {
        return Quat(x + o.x, y + o.y, z + o.z, w + o.w);
    }

    [[nodiscard]] constexpr Quat operator-(const Quat& o) const
    {
        return Quat(x - o.x, y - o.y, z - o.z, w - o.w);
    }

    [[nodiscard]] constexpr Quat operator-(const Element v) const
    {
        return Quat(x - v, y - v, z - v, w - v);
    }

    [[nodiscard]] constexpr Quat abs() const
    {
        return Quat(std::abs(x), std::abs(y), std::abs(z), std::abs(w));
    }

    [[nodiscard]] constexpr Element dot(const Quat& o) const
    {
        return (x * o.x) + (y * o.y) + (z * o.z) + (w * o.w);
    }

    friend std::ostream& operator<<(std::ostream& os, const Quat& q)
    {
        os << R"("Quat" { "x": ")" << q.x << R"(", "y": ")" << q.y << R"(", "z": ")" << q.z << R"(", "w": ")" << q.w << "\" }";
        return os;
    }

    [[nodiscard]] constexpr bool equal(const Quat& o, const Element tolerance) const
    {
        const auto a = (*this - o).abs();
        return a.x < tolerance && a.y < tolerance && a.z < tolerance && a.w < tolerance;
    }
};
}
#endif

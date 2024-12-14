#pragma once
#include "gx-math-matrix-4d.hpp"
#include "gx-math-numeric.hpp"
#include <algorithm>
#include <ostream>

namespace gearoenix::math {
template <typename Element>
struct Quat final {
    Element x, y, z, w;

    constexpr Quat()
        : x(static_cast<Element>(0))
        , y(static_cast<Element>(0))
        , z(static_cast<Element>(0))
        , w(static_cast<Element>(1))
    {
    }

    constexpr explicit Quat(const Element e)
        : x(e)
        , y(e)
        , z(e)
        , w(static_cast<Element>(1))
    {
    }

    constexpr Quat(const Element x, const Element y, const Element z, const Element w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    constexpr Quat(const Vec3<Element>& xyz, const Element w)
        : x(xyz.x)
        , y(xyz.y)
        , z(xyz.z)
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

    constexpr Mat4x4<Element> to_m4x4() const
    {
        return Mat4x4(to_m3x3());
    }

    constexpr Mat3x3<Element> to_m3x3() const
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

        return {
            { static_cast<Element>(1) - static_cast<Element>(2) * (yy + zz),
                static_cast<Element>(2) * (xy + zw),
                static_cast<Element>(2) * (xz - yw) },
            { static_cast<Element>(2) * (xy - zw),
                static_cast<Element>(1) - static_cast<Element>(2) * (xx + zz),
                static_cast<Element>(2) * (yz + xw) },
            { static_cast<Element>(2) * (xz + yw),
                static_cast<Element>(2) * (yz - xw),
                static_cast<Element>(1) - static_cast<Element>(2) * (xx + yy) }
        };
    }

    [[nodiscard]] constexpr bool safe_equal(const Quat& o) const
    {
        const auto nt = normalised();
        const auto no = o.normalised();
        return Numeric::equal(nt.x, no.x) && Numeric::equal(nt.y, no.y) && Numeric::equal(nt.z, no.z) && Numeric::equal(nt.w, no.w);
    }

    [[nodiscard]] constexpr Quat conjugate() const
    {
        return { -x, -y, -z, w };
    }

    [[nodiscard]] constexpr Vec3<Element> rotate(const Vec3<Element>& v) const
    {
        const auto q = (*this * v) * conjugate();
        return { q.x, q.y, q.z };
    }

    [[nodiscard]] constexpr Quat operator*(const Element v) const
    {
        return { x * v, y * v, z * v, w * v };
    }

    [[nodiscard]] constexpr Quat operator*(const Quat& o) const
    {
        return {
            w * o.x + x * o.w + y * o.z - z * o.y,
            w * o.y + y * o.w + z * o.x - x * o.z,
            w * o.z + z * o.w + x * o.y - y * o.x,
            w * o.w - x * o.x - y * o.y - z * o.z
        };
    }

    constexpr void operator*=(const Quat& o)
    {
        *this = *this * o;
    }

    [[nodiscard]] constexpr Quat operator*(const Vec3<Element>& o) const
    {
        return {
            w * o.x + y * o.z - z * o.y,
            w * o.y + z * o.x - x * o.z,
            w * o.z + x * o.y - y * o.x,
            -x * o.x - y * o.y - z * o.z
        };
    }

    [[nodiscard]] constexpr Quat operator+(const Quat& o) const
    {
        return { x + o.x, y + o.y, z + o.z, w + o.w };
    }

    [[nodiscard]] constexpr Quat operator-(const Quat& o) const
    {
        return { x - o.x, y - o.y, z - o.z, w - o.w };
    }

    [[nodiscard]] constexpr Quat operator-(const Element v) const
    {
        return { x - v, y - v, z - v, w - v };
    }

    [[nodiscard]] constexpr Quat abs() const
    {
        return { std::abs(x), std::abs(y), std::abs(z), std::abs(w) };
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

    void write(platform::stream::Stream& s) const
    {
        s.write_fail_debug(x);
        s.write_fail_debug(y);
        s.write_fail_debug(z);
        s.write_fail_debug(w);
    }

    void read(platform::stream::Stream& s)
    {
        s.read(x);
        s.read(y);
        s.read(z);
        s.read(w);
    }

    [[nodiscard]] constexpr bool equal(const Quat& o, const Element tolerance = Numeric::epsilon<Element>) const
    {
        const auto a = (*this - o).abs();
        return a.x < tolerance && a.y < tolerance && a.z < tolerance && a.w < tolerance;
    }

    [[nodiscard]] Vec3<Element> to_euler() const
    {
        Vec3<Element> angles;

        const auto pitch_atan_xy = Vec2<Element>(
            static_cast<Element>(2) * (w * x + y * z),
            w * w - x * x - y * y + z * z);
        if (pitch_atan_xy.equal(static_cast<Element>(0))) {
            angles.x = static_cast<Element>(2) * std::atan2(x, w);
        } else {
            angles.x = static_cast<Element>(std::atan2(pitch_atan_xy.x, pitch_atan_xy.y));
        }

        const auto sin_p = std::clamp(static_cast<Element>(2) * (w * y - x * z), static_cast<Element>(-1), static_cast<Element>(1));
        angles.y = std::asin(sin_p);

        const auto roll_atan_xy = Vec2<Element>(
            static_cast<Element>(2) * (w * z + x * y),
            w * w + x * x - y * y - z * z);
        if (roll_atan_xy.equal(static_cast<Element>(0))) {
            angles.z = static_cast<Element>(0);
        } else {
            angles.z = static_cast<Element>(std::atan2(roll_atan_xy.x, roll_atan_xy.y));
        }

        return angles;
    }

    [[nodiscard]] Vec3<Element> to_euler_degree() const
    {
        return to_euler() * (180.0 / std::numbers::pi);
    }

    [[nodiscard]] constexpr static Quat from_euler(const Element x, const Element y, const Element z)
    {
        const auto cr = static_cast<Element>(std::cos(x * static_cast<Element>(0.5)));
        const auto sr = static_cast<Element>(std::sin(x * static_cast<Element>(0.5)));
        const auto cp = static_cast<Element>(std::cos(y * static_cast<Element>(0.5)));
        const auto sp = static_cast<Element>(std::sin(y * static_cast<Element>(0.5)));
        const auto cy = static_cast<Element>(std::cos(z * static_cast<Element>(0.5)));
        const auto sy = static_cast<Element>(std::sin(z * static_cast<Element>(0.5)));

        return {
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
            cr * cp * cy + sr * sp * sy
        };
    }

    [[nodiscard]] constexpr static Quat from_euler(Vec3<Element> angles)
    {
        return from_euler(angles.x, angles.y, angles.z);
    }

    [[nodiscard]] constexpr static Quat from_euler_degree(Vec3<Element> angles)
    {
        return from_euler(angles * static_cast<Element>(std::numbers::pi / 180.0));
    }

    [[nodiscard]] constexpr static Quat from(const Vec3<Element>& x, const Vec3<Element>& y, const Vec3<Element>& z)
    {
        const auto trace0 = x.x - y.y - z.z;
        const auto trace1 = y.y - x.x - z.z;
        const auto trace2 = z.z - x.x - y.y;
        const auto trace3 = x.x + y.y + z.z;

        int biggest_index = 3;
        auto biggest = trace3;
        if (trace0 > biggest) {
            biggest = trace0;
            biggest_index = 0;
        }
        if (trace1 > biggest) {
            biggest = trace1;
            biggest_index = 1;
        }
        if (trace2 > biggest) {
            biggest = trace2;
            biggest_index = 2;
        }

        biggest = sqrt(biggest + static_cast<Element>(1)) * static_cast<Element>(0.5);
        const auto mlt = static_cast<Element>(0.25) / biggest;

        switch (biggest_index) {
        case 0:
            return { biggest, (x.y + y.x) * mlt, (z.x + x.z) * mlt, (y.z - z.y) * mlt };
        case 1:
            return { (x.y + y.x) * mlt, biggest, (y.z + z.y) * mlt, (z.x - x.z) * mlt };
        case 2:
            return { (z.x + x.z) * mlt, (y.z + z.y) * mlt, biggest, (x.y - y.x) * mlt };
        case 3:
            return { (y.z - z.y) * mlt, (z.x - x.z) * mlt, (x.y - y.x) * mlt, biggest };
        default:
            return { 0, 0, 0, 0 };
        }
    }

    [[nodiscard]] constexpr static Quat from(const Mat4x4<Element>& m)
    {
        Vec3<Element> x, y, z;
        m.get_axes(x, y, z);

        return from(x, y, z);
    }

    [[nodiscard]] constexpr static Quat from(const Mat3x3<Element>& m)
    {
        return from(m[0], m[1], m[2]);
    }

    [[nodiscard]] constexpr static Quat angle_axis(const Element rad, const Vec3<Element>& axis)
    {
        const auto ha = static_cast<Element>(0.5) * rad;
        return { axis * std::sin(ha), std::cos(ha) };
    }
};
}

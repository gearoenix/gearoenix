#ifndef GEAROENIX_MATH_MATRIX_4D_HPP
#define GEAROENIX_MATH_MATRIX_4D_HPP
#include "math-vector-4d.hpp"

namespace gearoenix::math {
/// It is a column major matrix
template <typename Element>
struct Mat4x4 {
    Element data[4][4];

    constexpr explicit Mat4x4(const Element diameter = static_cast<Element>(1)) noexcept
        : data {
            { diameter, static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0) },
            { static_cast<Element>(0), diameter, static_cast<Element>(0), static_cast<Element>(0) },
            { static_cast<Element>(0), static_cast<Element>(0), diameter, static_cast<Element>(0) },
            { static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(1) }
        }
    {
    }

    constexpr Mat4x4(
        const Element e0, const Element e1, const Element e2, const Element e3,
        const Element e4, const Element e5, const Element e6, const Element e7,
        const Element e8, const Element e9, const Element e10, const Element e11,
        const Element e12, const Element e13, const Element e14, const Element e15) noexcept
        : data {
            { e0, e1, e2, e3 },
            { e4, e5, e6, e7 },
            { e8, e9, e10, e11 },
            { e12, e13, e14, e15 },
        }
    {
    }

    explicit Mat4x4(system::stream::Stream* const f) noexcept
    {
        read(f);
    }

    constexpr Mat4x4(const Mat4x4<Element>& m) noexcept
        : data {
            { m.data[0][0], m.data[0][1], m.data[0][2], m.data[0][3] },
            { m.data[1][0], m.data[1][1], m.data[1][2], m.data[1][3] },
            { m.data[2][0], m.data[2][1], m.data[2][2], m.data[2][3] },
            { m.data[3][0], m.data[3][1], m.data[3][2], m.data[3][3] },
        }
    {
    }

    [[nodiscard]] constexpr Vec4<Element> operator*(const Vec4<Element>& v) const noexcept
    {
        return Vec4<Element>(
            data[0][0] * v.x + data[1][0] * v.y + data[2][0] * v.z + data[3][0] * v.w,
            data[0][1] * v.x + data[1][1] * v.y + data[2][1] * v.z + data[3][1] * v.w,
            data[0][2] * v.x + data[1][2] * v.y + data[2][2] * v.z + data[3][2] * v.w,
            data[0][3] * v.x + data[1][3] * v.y + data[2][3] * v.z + data[3][3] * v.w);
    }

    [[nodiscard]] constexpr Mat4x4<Element> operator*(const Mat4x4<Element>& m) const noexcept
    {
        Mat4x4<Element> r;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                r.data[i][j] = m.data[i][0] * data[0][j] + m.data[i][1] * data[1][j] + m.data[i][2] * data[2][j] + m.data[i][3] * data[3][j];
            }
        }
        return r;
    }

    constexpr Mat4x4<Element>& operator=(const Mat4x4<Element>& m) noexcept
    {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                data[i][j] = m.data[i][j];
            }
        }
        return *this;
    }

    constexpr void operator*=(const Mat4x4<Element>& m) noexcept
    {
        const auto o = *this * m;
        *this = o;
    }

    template <typename T>
    constexpr Element operator[](const T i) const noexcept
    {
        return data[i >> 2][i & 3];
    }

    template <typename T>
    constexpr Element& operator[](const T i) noexcept
    {
        return data[i >> 2][i & 3];
    }

    /// It does not change location
    constexpr void local_scale(const Element s) noexcept
    {
        local_scale(s, s, s);
    }

    /// It does not change location
    constexpr void local_x_scale(const Element s) noexcept
    {
        data[0][0] *= s;
        data[0][1] *= s;
        data[0][2] *= s;
        data[0][3] *= s;
    }

    /// It does not change location
    constexpr void local_y_scale(const Element s) noexcept
    {
        data[1][0] *= s;
        data[1][1] *= s;
        data[1][2] *= s;
        data[1][3] *= s;
    }

    /// It does not change location
    constexpr void local_z_scale(const Element s) noexcept
    {
        data[2][0] *= s;
        data[2][1] *= s;
        data[2][2] *= s;
        data[2][3] *= s;
    }

    /// It does not change location
    constexpr void local_w_scale(const Element s) noexcept
    {
        data[3][0] *= s;
        data[3][1] *= s;
        data[3][2] *= s;
        data[3][3] *= s;
    }

    /// It does not change location
    constexpr void local_scale(const Element a, const Element b, const Element c) noexcept
    {
        local_x_scale(a);
        local_y_scale(b);
        local_z_scale(c);
    }

    /// It changes location
    constexpr void local_scale(const Element a, const Element b, const Element c, const Element d) noexcept
    {
        local_scale(a, b, c);
        local_w_scale(d);
    }

    /// It does not change location
    constexpr void local_scale(const Vec3<Element>& s) noexcept
    {
        local_scale(s.x, s.y, s.z);
    }

    /// It changes location
    constexpr void local_scale(const Vec4<Element>& s) noexcept
    {
        local_scale(s.x, s.y, s.z, s.w);
    }

    /// It changes location
    constexpr void global_scale(const Element s) noexcept
    {
        global_scale(s, s, s);
    }

    /// It changes location
    constexpr void global_scale(const Element a, const Element b, const Element c) noexcept
    {
        data[0][0] *= a;
        data[1][0] *= a;
        data[2][0] *= a;
        data[3][0] *= a;

        data[0][1] *= b;
        data[1][1] *= b;
        data[2][1] *= b;
        data[3][1] *= b;

        data[0][2] *= c;
        data[1][2] *= c;
        data[2][2] *= c;
        data[3][2] *= c;
    }

    /// It changes location
    constexpr void global_scale(const Element a, const Element b, const Element c, const Element d) noexcept
    {
        global_scale(a, b, c);

        data[0][3] *= d;
        data[1][3] *= d;
        data[2][3] *= d;
        data[3][3] *= d;
    }

    /// It changes location
    constexpr void global_scale(const Vec3<Element>& s) noexcept
    {
        global_scale(s.x, s.y, s.z);
    }

    /// It changes location
    constexpr void global_scale(const Vec4<Element>& s) noexcept
    {
        global_scale(s.x, s.y, s.z, s.w);
    }

    constexpr void translate(const Vec3<Element>& v) noexcept
    {
        data[3][0] += v.x;
        data[3][1] += v.y;
        data[3][2] += v.z;
    }

    constexpr void set_location(const Vec3<Element>& location = Vec3<Element>(0)) noexcept
    {
        data[3][0] = location.x;
        data[3][1] = location.y;
        data[3][2] = location.z;
    }

    constexpr void get_location(Vec3<Element>& location) const noexcept
    {
        location.x = data[3][0];
        location.y = data[3][1];
        location.z = data[3][2];
    }

    [[nodiscard]] constexpr Vec3<Element> get_location() const noexcept
    {
        Vec3<Element> v;
        get_location(v);
        return v;
    }

    constexpr void inverse() noexcept
    {
        *this = inverted();
    }

    constexpr void transpose() noexcept
    {
        for (auto i = 0; i < 4; ++i) {
            for (auto j = i + 1; j < 4; ++j) {
                std::swap(data[i][j], data[j][i]);
            }
        }
    }

    void read(system::stream::Stream* const f) noexcept
    {
        for (Element& i : data)
            f->read(i);
    }

    [[nodiscard]] constexpr Element determinant() const noexcept
    {
        return (+data[0][0] * (+data[1][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) - data[2][1] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) + data[3][1] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]))
            - data[1][0] * (+data[0][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) - data[2][1] * (data[0][2] * data[3][3] - data[0][3] * data[3][2]) + data[3][1] * (data[0][2] * data[2][3] - data[0][3] * data[2][2]))
            + data[2][0] * (+data[0][1] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) - data[1][1] * (data[0][2] * data[3][3] - data[0][3] * data[3][2]) + data[3][1] * (data[0][2] * data[1][3] - data[0][3] * data[1][2]))
            - data[3][0] * (+data[0][1] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]) - data[1][1] * (data[0][2] * data[2][3] - data[0][3] * data[2][2]) + data[2][1] * (data[0][2] * data[1][3] - data[0][3] * data[1][2])));
    }

    [[nodiscard]] constexpr Mat4x4<Element> inverted() const noexcept
    {
        const auto id = static_cast<Element>(1) / determinant();
        Mat4x4<Element> result;
        result.data[0][0] = id * (+data[1][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) - data[2][1] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) + data[3][1] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]));
        result.data[0][1] = id * -(+data[0][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) - data[2][1] * (data[0][2] * data[3][3] - data[0][3] * data[3][2]) + data[3][1] * (data[0][2] * data[2][3] - data[0][3] * data[2][2]));
        result.data[0][2] = id * (+data[0][1] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) - data[1][1] * (data[0][2] * data[3][3] - data[0][3] * data[3][2]) + data[3][1] * (data[0][2] * data[1][3] - data[0][3] * data[1][2]));
        result.data[0][3] = id * -(+data[0][1] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]) - data[1][1] * (data[0][2] * data[2][3] - data[0][3] * data[2][2]) + data[2][1] * (data[0][2] * data[1][3] - data[0][3] * data[1][2]));
        result.data[1][0] = id * -(+data[1][0] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) - data[2][0] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) + data[3][0] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]));
        result.data[1][1] = id * (+data[0][0] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) - data[2][0] * (data[0][2] * data[3][3] - data[0][3] * data[3][2]) + data[3][0] * (data[0][2] * data[2][3] - data[0][3] * data[2][2]));
        result.data[1][2] = id * -(+data[0][0] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) - data[1][0] * (data[0][2] * data[3][3] - data[0][3] * data[3][2]) + data[3][0] * (data[0][2] * data[1][3] - data[0][3] * data[1][2]));
        result.data[1][3] = id * (+data[0][0] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]) - data[1][0] * (data[0][2] * data[2][3] - data[0][3] * data[2][2]) + data[2][0] * (data[0][2] * data[1][3] - data[0][3] * data[1][2]));
        result.data[2][0] = id * (+data[1][0] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) - data[2][0] * (data[1][1] * data[3][3] - data[1][3] * data[3][1]) + data[3][0] * (data[1][1] * data[2][3] - data[1][3] * data[2][1]));
        result.data[2][1] = id * -(+data[0][0] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) - data[2][0] * (data[0][1] * data[3][3] - data[0][3] * data[3][1]) + data[3][0] * (data[0][1] * data[2][3] - data[0][3] * data[2][1]));
        result.data[2][2] = id * (+data[0][0] * (data[1][1] * data[3][3] - data[1][3] * data[3][1]) - data[1][0] * (data[0][1] * data[3][3] - data[0][3] * data[3][1]) + data[3][0] * (data[0][1] * data[1][3] - data[0][3] * data[1][1]));
        result.data[2][3] = id * -(+data[0][0] * (data[1][1] * data[2][3] - data[1][3] * data[2][1]) - data[1][0] * (data[0][1] * data[2][3] - data[0][3] * data[2][1]) + data[2][0] * (data[0][1] * data[1][3] - data[0][3] * data[1][1]));
        result.data[3][0] = id * -(+data[1][0] * (data[2][1] * data[3][2] - data[2][2] * data[3][1]) - data[2][0] * (data[1][1] * data[3][2] - data[1][2] * data[3][1]) + data[3][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1]));
        result.data[3][1] = id * (+data[0][0] * (data[2][1] * data[3][2] - data[2][2] * data[3][1]) - data[2][0] * (data[0][1] * data[3][2] - data[0][2] * data[3][1]) + data[3][0] * (data[0][1] * data[2][2] - data[0][2] * data[2][1]));
        result.data[3][2] = id * -(+data[0][0] * (data[1][1] * data[3][2] - data[1][2] * data[3][1]) - data[1][0] * (data[0][1] * data[3][2] - data[0][2] * data[3][1]) + data[3][0] * (data[0][1] * data[1][2] - data[0][2] * data[1][1]));
        result.data[3][3] = id * (+data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1]) - data[1][0] * (data[0][1] * data[2][2] - data[0][2] * data[2][1]) + data[2][0] * (data[0][1] * data[1][2] - data[0][2] * data[1][1]));
        return result;
    }

    [[nodiscard]] constexpr Mat4x4<Element> transposed() const noexcept
    {
        Mat4x4<Element> r;
        for (auto i = 0; i < 4; ++i) {
            for (auto j = 0; j < 4; ++j) {
                r.data[i][j] = data[j][i];
            }
        }
        return r;
    }

    [[nodiscard]] constexpr Vec3<Element> project(const Vec3<Element>& v) const noexcept
    {
        Vec4<Element> v4(v, static_cast<Element>(1));
        v4 = *this * v4;
        return v4.xyz() / v4.w;
    }

    [[nodiscard]] constexpr static Mat4x4<Element> look_at(const Vec3<Element>& position, const Vec3<Element>& target, const Vec3<Element>& up) noexcept
    {
        const auto z = (target - position).normalized();
        const auto x = up.cross(z).normalized();
        const auto y = z.cross(x);
        Mat4x4<Element> m;
        m.data[0][0] = -x.x;
        m.data[0][1] = y.x;
        m.data[0][2] = -z.x;
        m.data[0][3] = static_cast<Element>(0);
        m.data[1][0] = -x.y;
        m.data[1][1] = y.y;
        m.data[1][2] = -z.y;
        m.data[1][3] = static_cast<Element>(0);
        m.data[2][0] = -x.z;
        m.data[2][1] = y.z;
        m.data[2][2] = -z.z;
        m.data[2][3] = static_cast<Element>(0);
        m.data[3][0] = x.dot(position);
        m.data[3][1] = -y.dot(position);
        m.data[3][2] = z.dot(position);
        m.data[3][3] = static_cast<Element>(1);
        return m;
    }

    [[nodiscard]] constexpr static Mat4x4<Element> rotation(const Vec3<Element>& v, const Element degree) noexcept
    {
        const auto sinus = static_cast<core::Real>(sin(static_cast<double>(degree)));
        const auto cosinus = static_cast<core::Real>(cos(static_cast<double>(degree)));
        const core::Real oneminuscos = 1.0f - cosinus;
        const Vec3 w = v.normalized();
        const core::Real wx2 = w[0] * w[0];
        const core::Real wxy = w[0] * w[1];
        const core::Real wxz = w[0] * w[2];
        const core::Real wy2 = w[1] * w[1];
        const core::Real wyz = w[1] * w[2];
        const core::Real wz2 = w[2] * w[2];
        const core::Real wxyonemincos = wxy * oneminuscos;
        const core::Real wxzonemincos = wxz * oneminuscos;
        const core::Real wyzonemincos = wyz * oneminuscos;
        const core::Real wxsin = w[0] * sinus;
        const core::Real wysin = w[1] * sinus;
        const core::Real wzsin = w[2] * sinus;
        Mat4x4 m;
        m.data[0][0] = cosinus + (wx2 * oneminuscos);
        m.data[0][1] = wzsin + wxyonemincos;
        m.data[0][2] = wxzonemincos - wysin;
        m.data[0][3] = static_cast<Element>(0);
        m.data[1][0] = wxyonemincos - wzsin;
        m.data[1][1] = cosinus + (wy2 * oneminuscos);
        m.data[1][2] = wxsin + wyzonemincos;
        m.data[1][3] = static_cast<Element>(0);
        m.data[2][0] = wysin + wxzonemincos;
        m.data[2][1] = wyzonemincos - wxsin;
        m.data[2][2] = cosinus + (wz2 * oneminuscos);
        m.data[2][3] = static_cast<Element>(0);
        m.data[3][0] = static_cast<Element>(0);
        m.data[3][1] = static_cast<Element>(0);
        m.data[3][2] = static_cast<Element>(0);
        m.data[3][3] = static_cast<Element>(1);
        return m;
    }

    [[nodiscard]] constexpr static Mat4x4<Element> translator(const Vec3<Element>& v) noexcept
    {
        Mat4x4<Element> r;
        r.data[0][0] = static_cast<Element>(1);
        r.data[0][1] = static_cast<Element>(0);
        r.data[0][2] = static_cast<Element>(0);
        r.data[0][3] = static_cast<Element>(0);
        r.data[1][0] = static_cast<Element>(0);
        r.data[1][1] = static_cast<Element>(1);
        r.data[1][2] = static_cast<Element>(0);
        r.data[1][3] = static_cast<Element>(0);
        r.data[2][0] = static_cast<Element>(0);
        r.data[2][1] = static_cast<Element>(0);
        r.data[2][2] = static_cast<Element>(1);
        r.data[2][3] = static_cast<Element>(0);
        r.data[3][0] = v.x;
        r.data[3][1] = v.y;
        r.data[3][2] = v.z;
        r.data[3][3] = static_cast<Element>(1);
        return r;
    }

    [[nodiscard]] constexpr static Mat4x4<Element> orthographic(const Element width, const Element height, const Element near, const Element far) noexcept
    {
        Mat4x4 r;
        r.data[0][0] = core::Real(2.0f / width);
        r.data[0][1] = static_cast<Element>(0);
        r.data[0][2] = static_cast<Element>(0);
        r.data[0][3] = static_cast<Element>(0);
        r.data[1][0] = static_cast<Element>(0);
        r.data[1][1] = core::Real(2.0f / height);
        r.data[1][2] = static_cast<Element>(0);
        r.data[1][3] = static_cast<Element>(0);
        r.data[2][0] = static_cast<Element>(0);
        r.data[2][1] = static_cast<Element>(0);
        r.data[2][2] = core::Real(2.0f / (near - far));
        r.data[2][3] = static_cast<Element>(0);
        r.data[3][0] = static_cast<Element>(0);
        r.data[3][1] = static_cast<Element>(0);
        r.data[3][2] = core::Real((far + near) / (near - far));
        r.data[3][3] = static_cast<Element>(1);
        return r;
    }

    [[nodiscard]] constexpr static Mat4x4<Element> perspective(const Element width, const Element height, const Element near, const Element far) noexcept
    {
        Mat4x4 r;
        r.data[0][0] = core::Real((2.0f * near) / width);
        r.data[0][1] = static_cast<Element>(0);
        r.data[0][2] = static_cast<Element>(0);
        r.data[0][3] = static_cast<Element>(0);
        r.data[1][0] = static_cast<Element>(0);
        r.data[1][1] = core::Real((2.0f * near) / height);
        r.data[1][2] = static_cast<Element>(0);
        r.data[1][3] = static_cast<Element>(0);
        r.data[2][0] = static_cast<Element>(0);
        r.data[2][1] = static_cast<Element>(0);
        r.data[2][2] = core::Real((far + near) / (near - far));
        r.data[2][3] = core::Real(-1.0);
        r.data[3][0] = static_cast<Element>(0);
        r.data[3][1] = static_cast<Element>(0);
        r.data[3][2] = core::Real((2.0f * far * near) / (near - far));
        r.data[3][3] = static_cast<Element>(0);
        return r;
    }
};
}
#endif

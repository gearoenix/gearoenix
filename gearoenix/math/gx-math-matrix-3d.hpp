#ifndef GEAROENIX_MATH_MATRIX_3D_HPP
#define GEAROENIX_MATH_MATRIX_3D_HPP
#include "gx-math-vector-3d.hpp"
#include <array>

namespace gearoenix::math {
/// It is a column major matrix
template <typename Element>
struct Mat3x3 final {
    std::array<Vec3<Element>, 3> columns;

    constexpr Mat3x3()
        : columns { { { 1, 0, 0 },
              { 0, 1, 0 },
              { 0, 0, 1 } } }
    {
    }

    constexpr Mat3x3(const Vec3<Element>& c0, const Vec3<Element>& c1, const Vec3<Element>& c2)
        : columns { c0, c1, c2 }
    {
    }

    constexpr explicit Mat3x3(const Element diameter)
        : columns { { { diameter, static_cast<Element>(0), static_cast<Element>(0) },
              { static_cast<Element>(0), diameter, static_cast<Element>(0) },
              { static_cast<Element>(0), static_cast<Element>(0), diameter } } }
    {
    }

    constexpr Mat3x3(
        const Element e0, const Element e1, const Element e2,
        const Element e3, const Element e4, const Element e5,
        const Element e6, const Element e7, const Element e8)
        : columns { { { e0, e1, e2 },
              { e3, e4, e5 },
              { e6, e7, e8 } } }
    {
    }

    explicit Mat3x3(platform::stream::Stream& s)
    {
        read(s);
    }

    constexpr Mat3x3(const Mat3x3& o)
        : columns(o.columns)
    {
    }

    template <typename T>
    constexpr explicit Mat3x3(const Mat3x3<T>& o)
        : columns {
            Vec3<Element>(o.columns[0]),
            Vec3<Element>(o.columns[1]),
            Vec3<Element>(o.columns[2]),
        }
    {
    }

    [[nodiscard]] constexpr Vec3<Element> operator*(const Vec3<Element>& v) const
    {
        return columns[0] * v.x + columns[1] * v.y + columns[2] * v.z;
    }

    [[nodiscard]] constexpr Mat3x3 operator*(const Mat3x3& o) const
    {
        Mat3x3 r;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                r.columns[i][j] = o.columns[i].x * columns[0][j] + o.columns[i].y * columns[1][j] + o.columns[i].z * columns[2][j];
            }
        }
        return r;
    }

    constexpr Mat3x3& operator=(const Mat3x3& o) = default;

    constexpr void operator*=(const Mat3x3& o)
    {
        *this = *this * o;
    }

    [[nodiscard]] constexpr bool operator==(const Mat3x3& o) const
    {
        return o.columns == columns;
    }

    template <typename T>
    constexpr const Vec3<Element>& operator[](const T i) const
    {
        return columns[i];
    }

    template <typename T>
    constexpr Vec3<Element>& operator[](const T i)
    {
        return columns[i];
    }

    constexpr void local_x_scale(const Element s)
    {
        columns[0] *= s;
    }

    constexpr void local_y_scale(const Element s)
    {
        columns[1] *= s;
    }

    constexpr void local_z_scale(const Element s)
    {
        columns[2] *= s;
    }

    constexpr void local_scale(const Element s)
    {
        local_scale(s, s, s);
    }

    constexpr void local_scale(const Element a, const Element b, const Element c)
    {
        local_x_scale(a);
        local_y_scale(b);
        local_z_scale(c);
    }

    constexpr void local_scale(const Vec3<Element>& s)
    {
        local_scale(s.x, s.y, s.z);
    }

    constexpr void global_scale(const Element s)
    {
        global_scale(s, s, s);
    }

    constexpr void global_x_scale(const Element v)
    {
        columns[0].x *= v;
        columns[1].x *= v;
        columns[2].x *= v;
    }

    constexpr void global_y_scale(const Element v)
    {
        columns[0].y *= v;
        columns[1].y *= v;
        columns[2].y *= v;
    }

    constexpr void global_z_scale(const Element v)
    {
        columns[0].z *= v;
        columns[1].z *= v;
        columns[2].z *= v;
    }

    constexpr void global_scale(const Element x, const Element y, const Element z)
    {
        global_x_scale(x);
        global_y_scale(y);
        global_z_scale(z);
    }

    constexpr void global_scale(const Vec3<Element>& s)
    {
        global_scale(s.x, s.y, s.z);
    }

    constexpr void inverse()
    {
        *this = inverted();
    }

    constexpr void transpose()
    {
        for (auto i = 0; i < 3; ++i) {
            for (auto j = i + 1; j < 3; ++j) {
                std::swap(columns[i][j], columns[j][i]);
            }
        }
    }

    void read(platform::stream::Stream& s)
    {
        for (auto& c : columns) {
            c.read(s);
        }
    }

    [[nodiscard]] constexpr Element determinant() const
    {
        return +columns[0].x * (columns[1].y * columns[2].z - columns[2].y * columns[1].z)
            - columns[1].x * (columns[0].y * columns[2].z - columns[2].y * columns[0].z)
            + columns[2].x * (columns[0].y * columns[1].z - columns[1].y * columns[0].z);
    }

    [[nodiscard]] constexpr Mat3x3 inverted() const
    {
        const auto id = static_cast<Element>(1) / determinant();
        Mat3x3 result;
        result.columns[0].x = (columns[1].y * columns[2].z - columns[2].y * columns[1].z) * id;
        result.columns[1].x = (columns[2].x * columns[1].z - columns[1].x * columns[2].z) * id;
        result.columns[2].x = (columns[1].x * columns[2].y - columns[2].x * columns[1].y) * id;
        result.columns[0].y = (columns[2].y * columns[0].z - columns[0].y * columns[2].z) * id;
        result.columns[1].y = (columns[0].x * columns[2].z - columns[2].x * columns[0].z) * id;
        result.columns[2].y = (columns[2].x * columns[0].y - columns[0].x * columns[2].y) * id;
        result.columns[0].z = (columns[0].y * columns[1].z - columns[1].y * columns[0].z) * id;
        result.columns[1].z = (columns[1].x * columns[0].z - columns[0].x * columns[1].z) * id;
        result.columns[2].z = (columns[0].x * columns[1].y - columns[1].x * columns[0].y) * id;
        return result;
    }

    [[nodiscard]] constexpr Mat3x3 transposed() const
    {
        Mat3x3 r = *this;
        r.transpose();
        return r;
    }

    constexpr void normalise(Vec3<Element>& scale)
    {
        scale = { columns[0].length(), columns[1].length(), columns[2].length() };
        columns[0] *= static_cast<Element>(1) / scale.x;
        columns[1] *= static_cast<Element>(1) / scale.y;
        columns[2] *= static_cast<Element>(1) / scale.z;
    }

    [[nodiscard]] constexpr static Mat3x3 look_at(const Vec3<Element>& position, const Vec3<Element>& target, const Vec3<Element>& up)
    {
        const auto z = (position - target).normalised();
        const auto x = up.cross(z).normalised();
        const auto y = z.cross(x);

        Mat3x3 m;

        m.columns[0].x = x.x;
        m.columns[1].x = x.y;
        m.columns[2].x = x.z;

        m.columns[0].y = y.x;
        m.columns[1].y = y.y;
        m.columns[2].y = y.z;

        m.columns[0].z = z.x;
        m.columns[1].z = z.y;
        m.columns[2].z = z.z;

        return m;
    }

    [[nodiscard]] constexpr static Mat3x3 look_at_inverted(const Vec3<Element>& position, const Vec3<Element>& target, const Vec3<Element>& up)
    {
        const auto z = (position - target).normalised();
        const auto x = up.cross(z).normalised();
        const auto y = z.cross(x);

        Mat3x3 m;

        m.columns[0] = x;
        m.columns[1] = y;
        m.columns[2] = z;

        return m;
    }

    [[nodiscard]] constexpr static Mat3x3 rotation(const Vec3<Element>& w, const Element degree)
    {
        const auto sinus = static_cast<Element>(sin(static_cast<double>(degree)));
        const auto cosine = static_cast<Element>(cos(static_cast<double>(degree)));
        const auto one_minus_cos = static_cast<Element>(1) - cosine;
        const auto wx2 = w.x * w.x;
        const auto wxy = w.x * w.y;
        const auto wxz = w.x * w.z;
        const auto wy2 = w.y * w.y;
        const auto wyz = w.y * w.z;
        const auto wz2 = w.z * w.z;
        const auto wxy_one_min_cos = wxy * one_minus_cos;
        const auto wxz_one_min_cos = wxz * one_minus_cos;
        const auto wyz_one_min_cos = wyz * one_minus_cos;
        const auto wx_sin = w.x * sinus;
        const auto wy_sin = w.y * sinus;
        const auto wz_sin = w.z * sinus;

        Mat3x3 m;
        m.columns[0].x = cosine + (wx2 * one_minus_cos);
        m.columns[0].y = wz_sin + wxy_one_min_cos;
        m.columns[0].z = wxz_one_min_cos - wy_sin;
        m.columns[1].x = wxy_one_min_cos - wz_sin;
        m.columns[1].y = cosine + (wy2 * one_minus_cos);
        m.columns[1].z = wx_sin + wyz_one_min_cos;
        m.columns[2].x = wy_sin + wxz_one_min_cos;
        m.columns[2].y = wyz_one_min_cos - wx_sin;
        m.columns[2].z = cosine + (wz2 * one_minus_cos);
        return m;
    }
};
}
#endif

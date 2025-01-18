#pragma once
#include "gx-math-matrix-3d.hpp"
#include "gx-math-vector-4d.hpp"

namespace gearoenix::math {
/// It is a column major matrix
template <typename Element>
struct Mat4x4 final {
    std::array<Vec4<Element>, 4> columns;

    constexpr Mat4x4()
        : columns { { { static_cast<Element>(1), static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0) },
              { static_cast<Element>(0), static_cast<Element>(1), static_cast<Element>(0), static_cast<Element>(0) },
              { static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(1), static_cast<Element>(0) },
              { static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(1) } } }
    {
    }

    constexpr explicit Mat4x4(const Element diameter)
        : columns { { { diameter, static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0) },
              { static_cast<Element>(0), diameter, static_cast<Element>(0), static_cast<Element>(0) },
              { static_cast<Element>(0), static_cast<Element>(0), diameter, static_cast<Element>(0) },
              { static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(1) } } }
    {
    }

    constexpr Mat4x4(
        const Element e0, const Element e1, const Element e2, const Element e3,
        const Element e4, const Element e5, const Element e6, const Element e7,
        const Element e8, const Element e9, const Element e10, const Element e11,
        const Element e12, const Element e13, const Element e14, const Element e15)
        : columns { {
              { e0, e1, e2, e3 },
              { e4, e5, e6, e7 },
              { e8, e9, e10, e11 },
              { e12, e13, e14, e15 },
          } }
    {
    }

    constexpr Mat4x4(const Vec4<Element>& c0, const Vec4<Element>& c1, const Vec4<Element>& c2, const Vec4<Element>& c3)
        : columns { { c0, c1, c2, c3 } }
    {
    }

    explicit Mat4x4(platform::stream::Stream& f)
    {
        read(f);
    }

    constexpr explicit Mat4x4(const Mat3x3<Element>& m)
        : columns { {
              { m[0], static_cast<Element>(0) },
              { m[1], static_cast<Element>(0) },
              { m[2], static_cast<Element>(0) },
              { static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(1) },
          } }
    {
    }

    constexpr Mat4x4(const Mat4x4& m) = default;

    template <typename T>
    constexpr explicit Mat4x4(const Mat4x4<T>& m)
        : columns { {
              { static_cast<Element>(m[0][0]), static_cast<Element>(m[0][1]), static_cast<Element>(m[0][2]), static_cast<Element>(m[0][3]) },
              { static_cast<Element>(m[1][0]), static_cast<Element>(m[1][1]), static_cast<Element>(m[1][2]), static_cast<Element>(m[1][3]) },
              { static_cast<Element>(m[2][0]), static_cast<Element>(m[2][1]), static_cast<Element>(m[2][2]), static_cast<Element>(m[2][3]) },
              { static_cast<Element>(m[3][0]), static_cast<Element>(m[3][1]), static_cast<Element>(m[3][2]), static_cast<Element>(m[3][3]) },
          } }
    {
    }

    [[nodiscard]] constexpr Vec4<Element> operator*(const Vec4<Element>& v) const
    {
        return {
            columns[0][0] * v.x + columns[1][0] * v.y + columns[2][0] * v.z + columns[3][0] * v.w,
            columns[0][1] * v.x + columns[1][1] * v.y + columns[2][1] * v.z + columns[3][1] * v.w,
            columns[0][2] * v.x + columns[1][2] * v.y + columns[2][2] * v.z + columns[3][2] * v.w,
            columns[0][3] * v.x + columns[1][3] * v.y + columns[2][3] * v.z + columns[3][3] * v.w
        };
    }

    [[nodiscard]] constexpr Mat4x4 operator*(const Element v) const
    {
        return {
            columns[0] * v,
            columns[1] * v,
            columns[2] * v,
            columns[3] * v,
        };
    }

    constexpr Mat4x4& operator*=(const Element v)
    {
        columns[0] *= v;
        columns[1] *= v;
        columns[2] *= v;
        columns[3] *= v;
        return *this;
    }

    [[nodiscard]] constexpr Mat4x4 operator*(const Mat4x4& m) const
    {
        Mat4x4 r;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                r[i][j] = m[i][0] * columns[0][j] + m[i][1] * columns[1][j] + m[i][2] * columns[2][j] + m[i][3] * columns[3][j];
            }
        }
        return r;
    }

    constexpr Mat4x4& operator=(const Mat4x4& m)
    {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                columns[i][j] = m[i][j];
            }
        }
        return *this;
    }

    constexpr Mat4x4& operator+=(const Mat4x4& o)
    {
        for (int i = 0; i < 4; ++i) {
            columns[i] += o.columns[i];
        }
        return *this;
    }

    [[nodiscard]] constexpr Mat4x4 operator+(const Mat4x4& o) const
    {
        Mat4x4 r;
        for (int i = 0; i < 4; ++i) {
            r.columns[i] = columns[i] + o.columns[i];
        }
        return r;
    }

    constexpr void operator*=(const Mat4x4& m)
    {
        const auto o = *this * m;
        *this = o;
    }

    [[nodiscard]] constexpr bool operator==(const Mat4x4& m) const
    {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (columns[i][j] != m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    template <typename T>
    constexpr const Vec4<Element>& operator[](const T i) const
    {
        return columns[i];
    }

    template <typename T>
    constexpr Vec4<Element>& operator[](const T i)
    {
        return columns[i];
    }

    /// It does not change position
    [[nodiscard]] constexpr Vec3<Element> get_diameter() const
    {
        return { columns[0].x, columns[1].y, columns[2].z };
    }

    /// It does not change position
    constexpr void local_scale(const Element s)
    {
        local_scale(s, s, s);
    }

    /// It does not change position
    constexpr void local_x_scale(const Element s)
    {
        columns[0] *= s;
    }

    /// It does not change position
    constexpr void local_y_scale(const Element s)
    {
        columns[1] *= s;
    }

    /// It does not change position
    constexpr void local_z_scale(const Element s)
    {
        columns[2] *= s;
    }

    /// It does not change position
    constexpr void local_w_scale(const Element s)
    {
        columns[3] *= s;
    }

    /// It does not change position
    constexpr void local_scale(const Element a, const Element b, const Element c)
    {
        local_x_scale(a);
        local_y_scale(b);
        local_z_scale(c);
    }

    /// It changes position
    constexpr void local_scale(const Element a, const Element b, const Element c, const Element d)
    {
        local_scale(a, b, c);
        local_w_scale(d);
    }

    /// It does not change position
    constexpr void local_scale(const Vec3<Element>& s)
    {
        local_scale(s.x, s.y, s.z);
    }

    /// It changes position
    constexpr void local_scale(const Vec4<Element>& s)
    {
        local_scale(s.x, s.y, s.z, s.w);
    }

    /// It changes position
    constexpr void global_scale(const Element s)
    {
        global_scale(s, s, s);
    }

    /// It changes position
    constexpr void global_scale(const Element a, const Element b, const Element c)
    {
        columns[0][0] *= a;
        columns[1][0] *= a;
        columns[2][0] *= a;
        columns[3][0] *= a;

        columns[0][1] *= b;
        columns[1][1] *= b;
        columns[2][1] *= b;
        columns[3][1] *= b;

        columns[0][2] *= c;
        columns[1][2] *= c;
        columns[2][2] *= c;
        columns[3][2] *= c;
    }

    /// It changes position
    constexpr void global_scale(const Element a, const Element b, const Element c, const Element d)
    {
        global_scale(a, b, c);

        columns[0][3] *= d;
        columns[1][3] *= d;
        columns[2][3] *= d;
        columns[3][3] *= d;
    }

    /// It changes position
    constexpr void global_scale(const Vec3<Element>& s)
    {
        global_scale(s.x, s.y, s.z);
    }

    /// It changes position
    constexpr void global_scale(const Vec4<Element>& s)
    {
        global_scale(s.x, s.y, s.z, s.w);
    }

    constexpr void translate(const Vec3<Element>& v)
    {
        columns[3].x += v.x;
        columns[3].y += v.y;
        columns[3].z += v.z;
    }

    constexpr void set_position(const Vec3<Element>& position = Vec3<Element>(0))
    {
        columns[3].x = position.x;
        columns[3].y = position.y;
        columns[3].z = position.z;
    }

    constexpr void get_position(Vec3<Element>& position) const
    {
        position = columns[3].xyz();
    }

    constexpr void get_axes(Vec3<Element>& x, Vec3<Element>& y, Vec3<Element>& z) const
    {
        x = columns[0].xyz();
        y = columns[1].xyz();
        z = columns[2].xyz();
    }

    constexpr void set_axes(const Vec3<Element>& x, const Vec3<Element>& y, const Vec3<Element>& z)
    {
        columns[0] = { x, columns[0].w };
        columns[1] = { y, columns[1].w };
        columns[2] = { z, columns[2].w };
    }

    [[nodiscard]] constexpr Vec3<Element> get_position() const
    {
        Vec3<Element> v;
        get_position(v);
        return v;
    }

    constexpr void set_m3x3(const Mat3x3<Element>& m)
    {
        columns[0] = { m[0], columns[0].w };
        columns[1] = { m[1], columns[1].w };
        columns[2] = { m[2], columns[2].w };
    }

    constexpr void inverse()
    {
        *this = inverted();
    }

    constexpr void transpose()
    {
        for (auto i = 0; i < 4; ++i) {
            for (auto j = i + 1; j < 4; ++j) {
                std::swap(columns[i][j], columns[j][i]);
            }
        }
    }

    void read(platform::stream::Stream& f)
    {
        for (auto& c : columns) {
            c.read(f);
        }
    }

    [[nodiscard]] constexpr Element determinant() const
    {
        return (+columns[0][0] * (+columns[1][1] * (columns[2][2] * columns[3][3] - columns[2][3] * columns[3][2]) - columns[2][1] * (columns[1][2] * columns[3][3] - columns[1][3] * columns[3][2]) + columns[3][1] * (columns[1][2] * columns[2][3] - columns[1][3] * columns[2][2]))
            - columns[1][0] * (+columns[0][1] * (columns[2][2] * columns[3][3] - columns[2][3] * columns[3][2]) - columns[2][1] * (columns[0][2] * columns[3][3] - columns[0][3] * columns[3][2]) + columns[3][1] * (columns[0][2] * columns[2][3] - columns[0][3] * columns[2][2]))
            + columns[2][0] * (+columns[0][1] * (columns[1][2] * columns[3][3] - columns[1][3] * columns[3][2]) - columns[1][1] * (columns[0][2] * columns[3][3] - columns[0][3] * columns[3][2]) + columns[3][1] * (columns[0][2] * columns[1][3] - columns[0][3] * columns[1][2]))
            - columns[3][0] * (+columns[0][1] * (columns[1][2] * columns[2][3] - columns[1][3] * columns[2][2]) - columns[1][1] * (columns[0][2] * columns[2][3] - columns[0][3] * columns[2][2]) + columns[2][1] * (columns[0][2] * columns[1][3] - columns[0][3] * columns[1][2])));
    }

    [[nodiscard]] constexpr Mat4x4 inverted() const
    {
        const auto id = static_cast<Element>(1) / determinant();
        Mat4x4 result;
        result.columns[0][0] = id * (+columns[1][1] * (columns[2][2] * columns[3][3] - columns[2][3] * columns[3][2]) - columns[2][1] * (columns[1][2] * columns[3][3] - columns[1][3] * columns[3][2]) + columns[3][1] * (columns[1][2] * columns[2][3] - columns[1][3] * columns[2][2]));
        result.columns[0][1] = id * -(+columns[0][1] * (columns[2][2] * columns[3][3] - columns[2][3] * columns[3][2]) - columns[2][1] * (columns[0][2] * columns[3][3] - columns[0][3] * columns[3][2]) + columns[3][1] * (columns[0][2] * columns[2][3] - columns[0][3] * columns[2][2]));
        result.columns[0][2] = id * (+columns[0][1] * (columns[1][2] * columns[3][3] - columns[1][3] * columns[3][2]) - columns[1][1] * (columns[0][2] * columns[3][3] - columns[0][3] * columns[3][2]) + columns[3][1] * (columns[0][2] * columns[1][3] - columns[0][3] * columns[1][2]));
        result.columns[0][3] = id * -(+columns[0][1] * (columns[1][2] * columns[2][3] - columns[1][3] * columns[2][2]) - columns[1][1] * (columns[0][2] * columns[2][3] - columns[0][3] * columns[2][2]) + columns[2][1] * (columns[0][2] * columns[1][3] - columns[0][3] * columns[1][2]));
        result.columns[1][0] = id * -(+columns[1][0] * (columns[2][2] * columns[3][3] - columns[2][3] * columns[3][2]) - columns[2][0] * (columns[1][2] * columns[3][3] - columns[1][3] * columns[3][2]) + columns[3][0] * (columns[1][2] * columns[2][3] - columns[1][3] * columns[2][2]));
        result.columns[1][1] = id * (+columns[0][0] * (columns[2][2] * columns[3][3] - columns[2][3] * columns[3][2]) - columns[2][0] * (columns[0][2] * columns[3][3] - columns[0][3] * columns[3][2]) + columns[3][0] * (columns[0][2] * columns[2][3] - columns[0][3] * columns[2][2]));
        result.columns[1][2] = id * -(+columns[0][0] * (columns[1][2] * columns[3][3] - columns[1][3] * columns[3][2]) - columns[1][0] * (columns[0][2] * columns[3][3] - columns[0][3] * columns[3][2]) + columns[3][0] * (columns[0][2] * columns[1][3] - columns[0][3] * columns[1][2]));
        result.columns[1][3] = id * (+columns[0][0] * (columns[1][2] * columns[2][3] - columns[1][3] * columns[2][2]) - columns[1][0] * (columns[0][2] * columns[2][3] - columns[0][3] * columns[2][2]) + columns[2][0] * (columns[0][2] * columns[1][3] - columns[0][3] * columns[1][2]));
        result.columns[2][0] = id * (+columns[1][0] * (columns[2][1] * columns[3][3] - columns[2][3] * columns[3][1]) - columns[2][0] * (columns[1][1] * columns[3][3] - columns[1][3] * columns[3][1]) + columns[3][0] * (columns[1][1] * columns[2][3] - columns[1][3] * columns[2][1]));
        result.columns[2][1] = id * -(+columns[0][0] * (columns[2][1] * columns[3][3] - columns[2][3] * columns[3][1]) - columns[2][0] * (columns[0][1] * columns[3][3] - columns[0][3] * columns[3][1]) + columns[3][0] * (columns[0][1] * columns[2][3] - columns[0][3] * columns[2][1]));
        result.columns[2][2] = id * (+columns[0][0] * (columns[1][1] * columns[3][3] - columns[1][3] * columns[3][1]) - columns[1][0] * (columns[0][1] * columns[3][3] - columns[0][3] * columns[3][1]) + columns[3][0] * (columns[0][1] * columns[1][3] - columns[0][3] * columns[1][1]));
        result.columns[2][3] = id * -(+columns[0][0] * (columns[1][1] * columns[2][3] - columns[1][3] * columns[2][1]) - columns[1][0] * (columns[0][1] * columns[2][3] - columns[0][3] * columns[2][1]) + columns[2][0] * (columns[0][1] * columns[1][3] - columns[0][3] * columns[1][1]));
        result.columns[3][0] = id * -(+columns[1][0] * (columns[2][1] * columns[3][2] - columns[2][2] * columns[3][1]) - columns[2][0] * (columns[1][1] * columns[3][2] - columns[1][2] * columns[3][1]) + columns[3][0] * (columns[1][1] * columns[2][2] - columns[1][2] * columns[2][1]));
        result.columns[3][1] = id * (+columns[0][0] * (columns[2][1] * columns[3][2] - columns[2][2] * columns[3][1]) - columns[2][0] * (columns[0][1] * columns[3][2] - columns[0][2] * columns[3][1]) + columns[3][0] * (columns[0][1] * columns[2][2] - columns[0][2] * columns[2][1]));
        result.columns[3][2] = id * -(+columns[0][0] * (columns[1][1] * columns[3][2] - columns[1][2] * columns[3][1]) - columns[1][0] * (columns[0][1] * columns[3][2] - columns[0][2] * columns[3][1]) + columns[3][0] * (columns[0][1] * columns[1][2] - columns[0][2] * columns[1][1]));
        result.columns[3][3] = id * (+columns[0][0] * (columns[1][1] * columns[2][2] - columns[1][2] * columns[2][1]) - columns[1][0] * (columns[0][1] * columns[2][2] - columns[0][2] * columns[2][1]) + columns[2][0] * (columns[0][1] * columns[1][2] - columns[0][2] * columns[1][1]));
        return result;
    }

    [[nodiscard]] constexpr Mat4x4 transposed() const
    {
        Mat4x4 r;
        for (auto i = 0; i < 4; ++i) {
            for (auto j = 0; j < 4; ++j) {
                r[i][j] = columns[j][i];
            }
        }
        return r;
    }

    [[nodiscard]] constexpr Vec3<Element> project(const Vec3<Element>& v) const
    {
        Vec4<Element> v4(v, static_cast<Element>(1));
        v4 = *this * v4;
        return v4.xyz() / v4.w;
    }

    [[nodiscard]] constexpr Mat3x3<Element> to_m3x3() const
    {
        Mat3x3<Element> r;
        r[0] = columns[0].xyz();
        r[1] = columns[1].xyz();
        r[2] = columns[2].xyz();
        return r;
    }

    [[nodiscard]] constexpr static Mat4x4 look_at(const Vec3<Element>& position, const Vec3<Element>& target, const Vec3<Element>& up)
    {
        const auto z = (position - target).normalised();
        const auto x = up.cross(z).normalised();
        const auto y = z.cross(x);

        Mat4x4 m;

        m[0][0] = x.x;
        m[1][0] = x.y;
        m[2][0] = x.z;

        m[0][1] = y.x;
        m[1][1] = y.y;
        m[2][1] = y.z;

        m[0][2] = z.x;
        m[1][2] = z.y;
        m[2][2] = z.z;

        m[3][0] = -x.dot(position);
        m[3][1] = -y.dot(position);
        m[3][2] = -z.dot(position);

        m[0][3] = static_cast<Element>(0);
        m[1][3] = static_cast<Element>(0);
        m[2][3] = static_cast<Element>(0);
        m[3][3] = static_cast<Element>(1);

        return m;
    }

    [[nodiscard]] constexpr static Mat4x4 look_at_inverted(const Vec3<Element>& position, const Vec3<Element>& target, const Vec3<Element>& up)
    {
        const auto z = (position - target).normalised();
        const auto x = up.cross(z).normalised();
        const auto y = z.cross(x);

        Mat4x4 m;

        m.set_axes(x, y, z);
        m.set_position(position);

        return m;
    }

    [[nodiscard]] constexpr static Mat4x4 rotation(const Vec3<Element>& w, const Element degree)
    {
        const auto sinus = static_cast<Element>(sin(static_cast<double>(degree)));
        const auto cosine = static_cast<Element>(cos(static_cast<double>(degree)));
        const Element one_minus_cos = 1.0f - cosine;
        const Element wx2 = w[0] * w[0];
        const Element wxy = w[0] * w[1];
        const Element wxz = w[0] * w[2];
        const Element wy2 = w[1] * w[1];
        const Element wyz = w[1] * w[2];
        const Element wz2 = w[2] * w[2];
        const Element wxy_one_min_cos = wxy * one_minus_cos;
        const Element wxz_one_min_cos = wxz * one_minus_cos;
        const Element wyz_one_min_cos = wyz * one_minus_cos;
        const Element wx_sin = w[0] * sinus;
        const Element wy_sin = w[1] * sinus;
        const Element wz_sin = w[2] * sinus;
        Mat4x4 m;
        m[0][0] = cosine + (wx2 * one_minus_cos);
        m[0][1] = wz_sin + wxy_one_min_cos;
        m[0][2] = wxz_one_min_cos - wy_sin;
        m[0][3] = static_cast<Element>(0);
        m[1][0] = wxy_one_min_cos - wz_sin;
        m[1][1] = cosine + (wy2 * one_minus_cos);
        m[1][2] = wx_sin + wyz_one_min_cos;
        m[1][3] = static_cast<Element>(0);
        m[2][0] = wy_sin + wxz_one_min_cos;
        m[2][1] = wyz_one_min_cos - wx_sin;
        m[2][2] = cosine + (wz2 * one_minus_cos);
        m[2][3] = static_cast<Element>(0);
        m[3][0] = static_cast<Element>(0);
        m[3][1] = static_cast<Element>(0);
        m[3][2] = static_cast<Element>(0);
        m[3][3] = static_cast<Element>(1);
        return m;
    }

    [[nodiscard]] constexpr static Mat4x4 translator(const Vec3<Element>& v)
    {
        Mat4x4 r;
        r[0][0] = static_cast<Element>(1);
        r[0][1] = static_cast<Element>(0);
        r[0][2] = static_cast<Element>(0);
        r[0][3] = static_cast<Element>(0);
        r[1][0] = static_cast<Element>(0);
        r[1][1] = static_cast<Element>(1);
        r[1][2] = static_cast<Element>(0);
        r[1][3] = static_cast<Element>(0);
        r[2][0] = static_cast<Element>(0);
        r[2][1] = static_cast<Element>(0);
        r[2][2] = static_cast<Element>(1);
        r[2][3] = static_cast<Element>(0);
        r[3][0] = v.x;
        r[3][1] = v.y;
        r[3][2] = v.z;
        r[3][3] = static_cast<Element>(1);
        return r;
    }

    [[nodiscard]] constexpr static Mat4x4 orthographic(
        const Element proj_width,
        const Element proj_height,
        const Element proj_near,
        const Element proj_far)
    {
        Mat4x4 r;
        r[0][0] = static_cast<Element>(2) / proj_width;
        r[0][1] = static_cast<Element>(0);
        r[0][2] = static_cast<Element>(0);
        r[0][3] = static_cast<Element>(0);
        r[1][0] = static_cast<Element>(0);
        r[1][1] = static_cast<Element>(2) / proj_height;
        r[1][2] = static_cast<Element>(0);
        r[1][3] = static_cast<Element>(0);
        r[2][0] = static_cast<Element>(0);
        r[2][1] = static_cast<Element>(0);
        r[2][2] = static_cast<Element>(2) / (proj_near - proj_far);
        r[2][3] = static_cast<Element>(0);
        r[3][0] = static_cast<Element>(0);
        r[3][1] = static_cast<Element>(0);
        r[3][2] = (proj_far + proj_near) / (proj_near - proj_far);
        r[3][3] = static_cast<Element>(1);
        return r;
    }

    [[nodiscard]] constexpr static Mat4x4 perspective(
        const Element proj_width,
        const Element proj_height,
        const Element proj_near,
        const Element proj_far)
    {
        Mat4x4 r;
        r[0][0] = (static_cast<Element>(2) * proj_near) / proj_width;
        r[0][1] = static_cast<Element>(0);
        r[0][2] = static_cast<Element>(0);
        r[0][3] = static_cast<Element>(0);
        r[1][0] = static_cast<Element>(0);
        r[1][1] = (static_cast<Element>(2) * proj_near) / proj_height;
        r[1][2] = static_cast<Element>(0);
        r[1][3] = static_cast<Element>(0);
        r[2][0] = static_cast<Element>(0);
        r[2][1] = static_cast<Element>(0);
        r[2][2] = (proj_far + proj_near) / (proj_near - proj_far);
        r[2][3] = static_cast<Element>(-1);
        r[3][0] = static_cast<Element>(0);
        r[3][1] = static_cast<Element>(0);
        r[3][2] = (static_cast<Element>(2) * proj_far * proj_near) / (proj_near - proj_far);
        r[3][3] = static_cast<Element>(0);
        return r;
    }

    [[nodiscard]] constexpr const Element* data() const { return reinterpret_cast<const Element*>(this); }

    [[nodiscard]] constexpr Element* data() { return reinterpret_cast<Element*>(this); }
};
}

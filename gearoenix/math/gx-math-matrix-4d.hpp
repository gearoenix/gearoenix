#ifndef GEAROENIX_MATH_MATRIX_4D_HPP
#define GEAROENIX_MATH_MATRIX_4D_HPP
#include "gx-math-vector-4d.hpp"

namespace gearoenix::math {
/// It is a column major matrix
template <typename Element>
struct Mat4x4 final {
    Element data[4][4];

    constexpr Mat4x4()
        : data {
            { static_cast<Element>(1), static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0) },
            { static_cast<Element>(0), static_cast<Element>(1), static_cast<Element>(0), static_cast<Element>(0) },
            { static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(1), static_cast<Element>(0) },
            { static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(1) }
        }
    {
    }

    constexpr explicit Mat4x4(const Element diameter)
        : data {
            { diameter, static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0) },
            { static_cast<Element>(0), diameter, static_cast<Element>(0), static_cast<Element>(0) },
            { static_cast<Element>(0), static_cast<Element>(0), diameter, static_cast<Element>(0) },
            { static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(0), static_cast<Element>(1) }
        }
    {
    }

    constexpr Mat4x4(
        const Element e0, const Element e1, const Element e2, const Element e3,
        const Element e4, const Element e5, const Element e6, const Element e7,
        const Element e8, const Element e9, const Element e10, const Element e11,
        const Element e12, const Element e13, const Element e14, const Element e15)
        : data {
            { e0, e1, e2, e3 },
            { e4, e5, e6, e7 },
            { e8, e9, e10, e11 },
            { e12, e13, e14, e15 },
        }
    {
    }

    explicit Mat4x4(platform::stream::Stream* const f)
    {
        read(f);
    }

    constexpr Mat4x4(const Mat4x4& m)
        : data {
            { m.data[0][0], m.data[0][1], m.data[0][2], m.data[0][3] },
            { m.data[1][0], m.data[1][1], m.data[1][2], m.data[1][3] },
            { m.data[2][0], m.data[2][1], m.data[2][2], m.data[2][3] },
            { m.data[3][0], m.data[3][1], m.data[3][2], m.data[3][3] },
        }
    {
    }

    template <typename T>
    constexpr explicit Mat4x4(const Mat4x4<T>& m)
        : data {
            { static_cast<Element>(m.data[0][0]), static_cast<Element>(m.data[0][1]), static_cast<Element>(m.data[0][2]), static_cast<Element>(m.data[0][3]) },
            { static_cast<Element>(m.data[1][0]), static_cast<Element>(m.data[1][1]), static_cast<Element>(m.data[1][2]), static_cast<Element>(m.data[1][3]) },
            { static_cast<Element>(m.data[2][0]), static_cast<Element>(m.data[2][1]), static_cast<Element>(m.data[2][2]), static_cast<Element>(m.data[2][3]) },
            { static_cast<Element>(m.data[3][0]), static_cast<Element>(m.data[3][1]), static_cast<Element>(m.data[3][2]), static_cast<Element>(m.data[3][3]) },
        }
    {
    }

    [[nodiscard]] constexpr Vec4<Element> operator*(const Vec4<Element>& v) const
    {
        return Vec4<Element>(
            data[0][0] * v.x + data[1][0] * v.y + data[2][0] * v.z + data[3][0] * v.w,
            data[0][1] * v.x + data[1][1] * v.y + data[2][1] * v.z + data[3][1] * v.w,
            data[0][2] * v.x + data[1][2] * v.y + data[2][2] * v.z + data[3][2] * v.w,
            data[0][3] * v.x + data[1][3] * v.y + data[2][3] * v.z + data[3][3] * v.w);
    }

    [[nodiscard]] constexpr Mat4x4 operator*(const Mat4x4& m) const
    {
        Mat4x4 r;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                r.data[i][j] = m.data[i][0] * data[0][j] + m.data[i][1] * data[1][j] + m.data[i][2] * data[2][j] + m.data[i][3] * data[3][j];
            }
        }
        return r;
    }

    constexpr Mat4x4& operator=(const Mat4x4& m)
    {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                data[i][j] = m.data[i][j];
            }
        }
        return *this;
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
                if (data[i][j] != m.data[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    template <typename T>
    constexpr Element operator[](const T i) const
    {
        return data[i >> 2][i & 3];
    }

    template <typename T>
    constexpr Element& operator[](const T i)
    {
        return data[i >> 2][i & 3];
    }

    /// It does not change location
    constexpr void local_scale(const Element s)
    {
        local_scale(s, s, s);
    }

    /// It does not change location
    constexpr void local_x_scale(const Element s)
    {
        data[0][0] *= s;
        data[0][1] *= s;
        data[0][2] *= s;
        data[0][3] *= s;
    }

    /// It does not change location
    constexpr void local_y_scale(const Element s)
    {
        data[1][0] *= s;
        data[1][1] *= s;
        data[1][2] *= s;
        data[1][3] *= s;
    }

    /// It does not change location
    constexpr void local_z_scale(const Element s)
    {
        data[2][0] *= s;
        data[2][1] *= s;
        data[2][2] *= s;
        data[2][3] *= s;
    }

    /// It does not change location
    constexpr void local_w_scale(const Element s)
    {
        data[3][0] *= s;
        data[3][1] *= s;
        data[3][2] *= s;
        data[3][3] *= s;
    }

    /// It does not change location
    constexpr void local_scale(const Element a, const Element b, const Element c)
    {
        local_x_scale(a);
        local_y_scale(b);
        local_z_scale(c);
    }

    /// It changes location
    constexpr void local_scale(const Element a, const Element b, const Element c, const Element d)
    {
        local_scale(a, b, c);
        local_w_scale(d);
    }

    /// It does not change location
    constexpr void local_scale(const Vec3<Element>& s)
    {
        local_scale(s.x, s.y, s.z);
    }

    /// It changes location
    constexpr void local_scale(const Vec4<Element>& s)
    {
        local_scale(s.x, s.y, s.z, s.w);
    }

    /// It changes location
    constexpr void global_scale(const Element s)
    {
        global_scale(s, s, s);
    }

    /// It changes location
    constexpr void global_scale(const Element a, const Element b, const Element c)
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
    constexpr void global_scale(const Element a, const Element b, const Element c, const Element d)
    {
        global_scale(a, b, c);

        data[0][3] *= d;
        data[1][3] *= d;
        data[2][3] *= d;
        data[3][3] *= d;
    }

    /// It changes location
    constexpr void global_scale(const Vec3<Element>& s)
    {
        global_scale(s.x, s.y, s.z);
    }

    /// It changes location
    constexpr void global_scale(const Vec4<Element>& s)
    {
        global_scale(s.x, s.y, s.z, s.w);
    }

    constexpr void translate(const Vec3<Element>& v)
    {
        data[3][0] += v.x;
        data[3][1] += v.y;
        data[3][2] += v.z;
    }

    constexpr void set_location(const Vec3<Element>& location = Vec3<Element>(0))
    {
        data[3][0] = location.x;
        data[3][1] = location.y;
        data[3][2] = location.z;
    }

    constexpr void get_location(Vec3<Element>& location) const
    {
        location.x = data[3][0];
        location.y = data[3][1];
        location.z = data[3][2];
    }

    constexpr void get_axes(Vec3<Element>& x, Vec3<Element>& y, Vec3<Element>& z) const
    {
        x.x = data[0][0];
        x.y = data[0][1];
        x.z = data[0][2];

        y.x = data[1][0];
        y.y = data[1][1];
        y.z = data[1][2];

        z.x = data[2][0];
        z.y = data[2][1];
        z.z = data[2][2];
    }

    constexpr void set_axes(const Vec3<Element>& x, const Vec3<Element>& y, const Vec3<Element>& z)
    {
        data[0][0] = x.x;
        data[0][1] = x.y;
        data[0][2] = x.z;

        data[1][0] = y.x;
        data[1][1] = y.y;
        data[1][2] = y.z;

        data[2][0] = z.x;
        data[2][1] = z.y;
        data[2][2] = z.z;
    }

    [[nodiscard]] constexpr Vec3<Element> get_location() const
    {
        Vec3<Element> v;
        get_location(v);
        return v;
    }

    constexpr void inverse()
    {
        *this = inverted();
    }

    constexpr void transpose()
    {
        for (auto i = 0; i < 4; ++i) {
            for (auto j = i + 1; j < 4; ++j) {
                std::swap(data[i][j], data[j][i]);
            }
        }
    }

    void read(platform::stream::Stream* const f)
    {
        for (auto& r : data)
            for (auto& c : r)
                c = static_cast<Element>(f->read<float>());
    }

    [[nodiscard]] constexpr Element determinant() const
    {
        return (+data[0][0] * (+data[1][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) - data[2][1] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) + data[3][1] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]))
            - data[1][0] * (+data[0][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) - data[2][1] * (data[0][2] * data[3][3] - data[0][3] * data[3][2]) + data[3][1] * (data[0][2] * data[2][3] - data[0][3] * data[2][2]))
            + data[2][0] * (+data[0][1] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) - data[1][1] * (data[0][2] * data[3][3] - data[0][3] * data[3][2]) + data[3][1] * (data[0][2] * data[1][3] - data[0][3] * data[1][2]))
            - data[3][0] * (+data[0][1] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]) - data[1][1] * (data[0][2] * data[2][3] - data[0][3] * data[2][2]) + data[2][1] * (data[0][2] * data[1][3] - data[0][3] * data[1][2])));
    }

    [[nodiscard]] constexpr Mat4x4 inverted() const
    {
        const auto id = static_cast<Element>(1) / determinant();
        Mat4x4 result;
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

    [[nodiscard]] constexpr Mat4x4 transposed() const
    {
        Mat4x4 r;
        for (auto i = 0; i < 4; ++i) {
            for (auto j = 0; j < 4; ++j) {
                r.data[i][j] = data[j][i];
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

    [[nodiscard]] constexpr static Mat4x4 look_at(const Vec3<Element>& position, const Vec3<Element>& target, const Vec3<Element>& up)
    {
        const auto z = (position - target).normalised();
        const auto x = up.cross(z).normalised();
        const auto y = z.cross(x);

        Mat4x4 m;

        m.data[0][0] = x.x;
        m.data[1][0] = x.y;
        m.data[2][0] = x.z;

        m.data[0][1] = y.x;
        m.data[1][1] = y.y;
        m.data[2][1] = y.z;

        m.data[0][2] = z.x;
        m.data[1][2] = z.y;
        m.data[2][2] = z.z;

        m.data[3][0] = -x.dot(position);
        m.data[3][1] = -y.dot(position);
        m.data[3][2] = -z.dot(position);

        m.data[0][3] = static_cast<Element>(0);
        m.data[1][3] = static_cast<Element>(0);
        m.data[2][3] = static_cast<Element>(0);
        m.data[3][3] = static_cast<Element>(1);

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
        m.data[0][0] = cosine + (wx2 * one_minus_cos);
        m.data[0][1] = wz_sin + wxy_one_min_cos;
        m.data[0][2] = wxz_one_min_cos - wy_sin;
        m.data[0][3] = static_cast<Element>(0);
        m.data[1][0] = wxy_one_min_cos - wz_sin;
        m.data[1][1] = cosine + (wy2 * one_minus_cos);
        m.data[1][2] = wx_sin + wyz_one_min_cos;
        m.data[1][3] = static_cast<Element>(0);
        m.data[2][0] = wy_sin + wxz_one_min_cos;
        m.data[2][1] = wyz_one_min_cos - wx_sin;
        m.data[2][2] = cosine + (wz2 * one_minus_cos);
        m.data[2][3] = static_cast<Element>(0);
        m.data[3][0] = static_cast<Element>(0);
        m.data[3][1] = static_cast<Element>(0);
        m.data[3][2] = static_cast<Element>(0);
        m.data[3][3] = static_cast<Element>(1);
        return m;
    }

    [[nodiscard]] constexpr static Mat4x4 translator(const Vec3<Element>& v)
    {
        Mat4x4 r;
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

    [[nodiscard]] constexpr static Mat4x4 orthographic(
        const Element proj_width,
        const Element proj_height,
        const Element proj_near,
        const Element proj_far)
    {
        Mat4x4 r;
        r.data[0][0] = static_cast<Element>(2) / proj_width;
        r.data[0][1] = static_cast<Element>(0);
        r.data[0][2] = static_cast<Element>(0);
        r.data[0][3] = static_cast<Element>(0);
        r.data[1][0] = static_cast<Element>(0);
        r.data[1][1] = static_cast<Element>(2) / proj_height;
        r.data[1][2] = static_cast<Element>(0);
        r.data[1][3] = static_cast<Element>(0);
        r.data[2][0] = static_cast<Element>(0);
        r.data[2][1] = static_cast<Element>(0);
        r.data[2][2] = static_cast<Element>(2) / (proj_near - proj_far);
        r.data[2][3] = static_cast<Element>(0);
        r.data[3][0] = static_cast<Element>(0);
        r.data[3][1] = static_cast<Element>(0);
        r.data[3][2] = (proj_far + proj_near) / (proj_near - proj_far);
        r.data[3][3] = static_cast<Element>(1);
        return r;
    }

    [[nodiscard]] constexpr static Mat4x4 perspective(
        const Element proj_width,
        const Element proj_height,
        const Element proj_near,
        const Element proj_far)
    {
        Mat4x4 r;
        r.data[0][0] = (static_cast<Element>(2) * proj_near) / proj_width;
        r.data[0][1] = static_cast<Element>(0);
        r.data[0][2] = static_cast<Element>(0);
        r.data[0][3] = static_cast<Element>(0);
        r.data[1][0] = static_cast<Element>(0);
        r.data[1][1] = (static_cast<Element>(2) * proj_near) / proj_height;
        r.data[1][2] = static_cast<Element>(0);
        r.data[1][3] = static_cast<Element>(0);
        r.data[2][0] = static_cast<Element>(0);
        r.data[2][1] = static_cast<Element>(0);
        r.data[2][2] = (proj_far + proj_near) / (proj_near - proj_far);
        r.data[2][3] = static_cast<Element>(-1);
        r.data[3][0] = static_cast<Element>(0);
        r.data[3][1] = static_cast<Element>(0);
        r.data[3][2] = (static_cast<Element>(2) * proj_far * proj_near) / (proj_near - proj_far);
        r.data[3][3] = static_cast<Element>(0);
        return r;
    }
};
}
#endif

#ifndef GEAROEMIX_MATH_MATRIX_HPP
#define GEAROEMIX_MATH_MATRIX_HPP
#include <cmath>
namespace gearoenix {
namespace math {
    template <typename T = float>
    class Vec2;

    template <typename T = float>
    class Vec3;

    template <typename T = float>
    class Vec4;

    template <typename T = float>
    struct Mat4x4 {
    private:
        T mat[16];

    public:
        Mat4x4(T e)
        {
            mat[0] = e;
            mat[1] = T(0);
            mat[2] = T(0);
            mat[3] = T(0);
            mat[4] = T(0);
            mat[5] = e;
            mat[6] = T(0);
            mat[7] = T(0);
            mat[8] = T(0);
            mat[9] = T(0);
            mat[10] = e;
            mat[11] = T(0);
            mat[12] = T(0);
            mat[13] = T(0);
            mat[14] = T(0);
            mat[15] = e;
        }
        Mat4x4()
        {
            mat[0] = T(1);
            mat[1] = T(0);
            mat[2] = T(0);
            mat[3] = T(0);
            mat[4] = T(0);
            mat[5] = T(1);
            mat[6] = T(0);
            mat[7] = T(0);
            mat[8] = T(0);
            mat[9] = T(0);
            mat[10] = T(1);
            mat[11] = T(0);
            mat[12] = T(0);
            mat[13] = T(0);
            mat[14] = T(0);
            mat[15] = T(1);
        }
        Mat4x4(T e0, T e1, T e2, T e3, T e4, T e5, T e6, T e7, T e8, T e9, T e10, T e11, T e12, T e13, T e14, T e15)
        {
            mat[0] = e0;
            mat[1] = e1;
            mat[2] = e2;
            mat[3] = e3;
            mat[4] = e4;
            mat[5] = e5;
            mat[6] = e6;
            mat[7] = e7;
            mat[8] = e8;
            mat[9] = e9;
            mat[10] = e10;
            mat[11] = e11;
            mat[12] = e12;
            mat[13] = e13;
            mat[14] = e14;
            mat[15] = e15;
        }
        Mat4x4(const Mat4x4<T>& m)
        {
            for (int i = 0; i < 16; i++) {
                mat[i] = m.mat[i];
            }
        }
        Vec3<T> operator*(const Vec3<T>& v) const
        {
            return Vec3<T>(mat[0] * v.vec[0] + mat[4] * v.vec[1] + mat[8] * v.vec[2] + mat[12],
                mat[1] * v.vec[0] + mat[5] * v.vec[1] + mat[9] * v.vec[2] + mat[13],
                mat[2] * v.vec[0] + mat[6] * v.vec[1] + mat[10] * v.vec[2] + mat[14]);
        }
        Vec4<T> operator*(const Vec4<T>& v) const
        {
            return Vec4<T>(mat[0] * v.vec[0] + mat[4] * v.vec[1] + mat[8] * v.vec[2] + mat[12] * v.vec[3],
                mat[1] * v.vec[0] + mat[5] * v.vec[1] + mat[9] * v.vec[2] + mat[13] * v.vec[3],
                mat[2] * v.vec[0] + mat[6] * v.vec[1] + mat[10] * v.vec[2] + mat[14] * v.vec[3],
                mat[3] * v.vec[0] + mat[7] * v.vec[1] + mat[11] * v.vec[2] + mat[15] * v.vec[3]);
        }
        Mat4x4<T> operator*(const Mat4x4<T>& m) const
        {
            Mat4x4<T> r;
            for (int i = 0, ri = 0; i < 16; i += 4) {
                for (int j = 0; j < 4; j++, ri++) {
                    r.mat[ri] = m.mat[i] * mat[j] + m.mat[i + 1] * mat[j + 4] + m.mat[i + 2] * mat[j + 8] + m.mat[i + 3] * mat[j + 12];
                }
            }
            return r;
        }
        void operator=(const Mat4x4<T>& m)
        {
            for (int i = 0; i < 16; i++) {
                mat[i] = m.mat[i];
            }
        }
        void scale3x3(const T& s)
        {
            mat[0] *= s;
            mat[1] *= s;
            mat[2] *= s;
            mat[4] *= s;
            mat[5] *= s;
            mat[6] *= s;
            mat[8] *= s;
            mat[9] *= s;
            mat[10] *= s;
        }
        void scale3x3(const T& a, const T& b, const T& c)
        {
            mat[0] *= a;
            mat[1] *= a;
            mat[2] *= a;
            mat[4] *= b;
            mat[5] *= b;
            mat[6] *= b;
            mat[8] *= c;
            mat[9] *= c;
            mat[10] *= c;
        }
        void scale4x3(const T& s)
        {
            mat[0] *= s;
            mat[1] *= s;
            mat[2] *= s;
            mat[4] *= s;
            mat[5] *= s;
            mat[6] *= s;
            mat[8] *= s;
            mat[9] *= s;
            mat[10] *= s;
            mat[12] *= s;
            mat[13] *= s;
            mat[14] *= s;
        }
        T determinant() const
        {
            return (+mat[0] * (+mat[5] * (mat[10] * mat[15] - mat[11] * mat[14]) - mat[9] * (mat[6] * mat[15] - mat[7] * mat[14]) + mat[13] * (mat[6] * mat[11] - mat[7] * mat[10]))
                - mat[4] * (+mat[1] * (mat[10] * mat[15] - mat[11] * mat[14]) - mat[9] * (mat[2] * mat[15] - mat[3] * mat[14]) + mat[13] * (mat[2] * mat[11] - mat[3] * mat[10]))
                + mat[8] * (+mat[1] * (mat[6] * mat[15] - mat[7] * mat[14]) - mat[5] * (mat[2] * mat[15] - mat[3] * mat[14]) + mat[13] * (mat[2] * mat[7] - mat[3] * mat[6]))
                - mat[12] * (+mat[1] * (mat[6] * mat[11] - mat[7] * mat[10]) - mat[5] * (mat[2] * mat[11] - mat[3] * mat[10]) + mat[9] * (mat[2] * mat[7] - mat[3] * mat[6])));
        }
        void setLocationZero()
        {
            mat[12] = T(0);
            mat[13] = T(0);
            mat[14] = T(0);
        }
        void setLocation(const Vec3<T>& location)
        {
            mat[12] = location.vec[0];
            mat[13] = location.vec[1];
            mat[14] = location.vec[2];
        }
    };
    template <typename T = float>
    T dot(const Vec2<T>& a, const Vec2<T>& b)
    {
        return (b.vec[0] * a.vec[0]) + (b.vec[1] * a.vec[1]);
    }
    template <typename T = float>
    T dot(const Vec3<T>& a, const Vec3<T>& b)
    {
        return (b.vec[0] * a.vec[0]) + (b.vec[1] * a.vec[1]) + (b.vec[2] * a.vec[2]);
    }
    template <typename T = float>
    Mat4x4<T> orthographic(const T& width, const T& height, const T& near, const T& far)
    {
        Mat4x4<T> r;
        r.mat[0] = T(2.0 / width);
        r.mat[1] = T(0.0);
        r.mat[2] = T(0.0);
        r.mat[3] = T(0.0);
        r.mat[4] = T(0.0);
        r.mat[5] = T(2.0 / height);
        r.mat[6] = T(0.0);
        r.mat[7] = T(0.0);
        r.mat[8] = T(0.0);
        r.mat[9] = T(0.0);
        r.mat[10] = T(2.0 / (near - far));
        r.mat[11] = T(0.0);
        r.mat[12] = T(0.0);
        r.mat[13] = T(0.0);
        r.mat[14] = T((far + near) / (near - far));
        r.mat[15] = T(1.0);
        return r;
    }
    template <typename T = float>
    Vec3<T> cross(const Vec3<T>& a, const Vec3<T>& b)
    {
        return Vec3<T>((a.vec[1] * b.vec[2]) - (a.vec[2] * b.vec[1]),
            (a.vec[2] * b.vec[0]) - (a.vec[0] * b.vec[2]),
            (a.vec[0] * b.vec[1]) - (a.vec[1] * b.vec[0]));
    }
    template <typename T = float>
    Vec2<T> normalize(const Vec2<T>& a)
    {
        return a / (a.length());
    }
    template <typename T = float>
    Vec3<T> normalize(const Vec3<T>& a)
    {
        T len = T(sqrt(double(a.vec[0] * a.vec[0] + a.vec[1] * a.vec[1] + a.vec[2] * a.vec[2])));
        return Vec3<T>(a.vec[0] / len, a.vec[1] / len, a.vec[2] / len);
    }
    template <typename T = float>
    Mat4x4<T> createRotationMatrix(const Vec3<T>& v, const T& degree)
    {
        T sinus = T(sin(degree));
        T cosinus = T(cos(degree));
        T oneminuscos = T(1.0 - cosinus);
        Vec3<T> w = normalize(v);
        T wx2 = w.vec[0] * w.vec[0];
        T wxy = w.vec[0] * w.vec[1];
        T wxz = w.vec[0] * w.vec[2];
        T wy2 = w.vec[1] * w.vec[1];
        T wyz = w.vec[1] * w.vec[2];
        T wz2 = w.vec[2] * w.vec[2];
        T wxyonemincos = wxy * oneminuscos;
        T wxzonemincos = wxz * oneminuscos;
        T wyzonemincos = wyz * oneminuscos;
        T wxsin = w.vec[0] * sinus;
        T wysin = w.vec[1] * sinus;
        T wzsin = w.vec[2] * sinus;
        Mat4x4<T> m;
        m.mat[0] = cosinus + (wx2 * oneminuscos);
        m.mat[1] = wzsin + wxyonemincos;
        m.mat[2] = wxzonemincos - wysin;
        m.mat[3] = T(0.0);
        m.mat[4] = wxyonemincos - wzsin;
        m.mat[5] = cosinus + (wy2 * oneminuscos);
        m.mat[6] = wxsin + wyzonemincos;
        m.mat[7] = T(0.0);
        m.mat[8] = wysin + wxzonemincos;
        m.mat[9] = wyzonemincos - wxsin;
        m.mat[10] = cosinus + (wz2 * oneminuscos);
        m.mat[11] = T(0.0);
        m.mat[12] = T(0.0);
        m.mat[13] = T(0.0);
        m.mat[14] = T(0.0);
        m.mat[15] = T(1.0);
        return m;
    }
    template <typename T = float>
    Mat4x4<T> lookAt(const Vec3<T>& position, const Vec3<T>& target, const Vec3<T>& up)
    {
        Vec3<T> z = normalize(target - position);
        Vec3<T> x = normalize(cross(up, z));
        Vec3<T> y = cross(z, x);
        Mat4x4<T> m;
        m.mat[0] = -x.vec[0];
        m.mat[1] = y.vec[0];
        m.mat[2] = -z.vec[0];
        m.mat[3] = T(0.0);
        m.mat[4] = -x.vec[1];
        m.mat[5] = y.vec[1];
        m.mat[6] = -z.vec[1];
        m.mat[7] = T(0.0);
        m.mat[8] = -x.vec[2];
        m.mat[9] = y.vec[2];
        m.mat[10] = -z.vec[2];
        m.mat[11] = T(0.0);
        m.mat[12] = dot(x, position);
        m.mat[13] = dot(-y, position);
        m.mat[14] = dot(z, position);
        m.mat[15] = T(1.0);
        return m;
    }
    template <typename T = float>
    Mat4x4<T> createTranslateMatrix(const Vec3<T>& v)
    {
        Mat4x4<T> r;
        r.mat[0] = T(1);
        r.mat[1] = T(0);
        r.mat[2] = T(0);
        r.mat[3] = T(0);
        r.mat[4] = T(0);
        r.mat[5] = T(1);
        r.mat[6] = T(0);
        r.mat[7] = T(0);
        r.mat[8] = T(0);
        r.mat[9] = T(0);
        r.mat[10] = T(1);
        r.mat[11] = T(0);
        r.mat[12] = v.vec[0];
        r.mat[13] = v.vec[1];
        r.mat[14] = v.vec[2];
        r.mat[15] = T(1);
        return r;
    }
    template <typename T = float>
    Mat4x4<T> perspective(const T& width, const T& height, const T& near, const T& far)
    {
        Mat4x4<T> r;
        r.mat[0] = T((2.0 * near) / width);
        r.mat[1] = T(0.0);
        r.mat[2] = T(0.0);
        r.mat[3] = T(0.0);
        r.mat[4] = T(0.0);
        r.mat[5] = T((2.0 * near) / height);
        r.mat[6] = T(0.0);
        r.mat[7] = T(0.0);
        r.mat[8] = T(0.0);
        r.mat[9] = T(0.0);
        r.mat[10] = T((far + near) / (near - far));
        r.mat[11] = T(-1.0);
        r.mat[12] = T(0.0);
        r.mat[13] = T(0.0);
        r.mat[14] = T((2.0 * far * near) / (near - far));
        r.mat[15] = T(0.0);
        return r;
    }
    template <typename T = float>
    Mat4x4<T> transpose(const Mat4x4<T>& m)
    {
        Mat4x4<T> r;
        for (int i = 0; i < 16; ++i) {
            r.mat[((i & 3) << 2) + (i >> 2)] = m.mat[i];
        }
        return r;
    }
    template <typename T = float>
    Mat4x4<T> inverse(const Mat4x4<T>& m)
    {
        T id = 1.0f / m.determinant();
        Mat4x4<T> result;
        result.mat[0] = id * (+m.mat[5] * (m.mat[10] * m.mat[15] - m.mat[11] * m.mat[14]) - m.mat[9] * (m.mat[6] * m.mat[15] - m.mat[7] * m.mat[14]) + m.mat[13] * (m.mat[6] * m.mat[11] - m.mat[7] * m.mat[10]));
        result.mat[1] = id * -(+m.mat[1] * (m.mat[10] * m.mat[15] - m.mat[11] * m.mat[14]) - m.mat[9] * (m.mat[2] * m.mat[15] - m.mat[3] * m.mat[14]) + m.mat[13] * (m.mat[2] * m.mat[11] - m.mat[3] * m.mat[10]));
        result.mat[2] = id * (+m.mat[1] * (m.mat[6] * m.mat[15] - m.mat[7] * m.mat[14]) - m.mat[5] * (m.mat[2] * m.mat[15] - m.mat[3] * m.mat[14]) + m.mat[13] * (m.mat[2] * m.mat[7] - m.mat[3] * m.mat[6]));
        result.mat[3] = id * -(+m.mat[1] * (m.mat[6] * m.mat[11] - m.mat[7] * m.mat[10]) - m.mat[5] * (m.mat[2] * m.mat[11] - m.mat[3] * m.mat[10]) + m.mat[9] * (m.mat[2] * m.mat[7] - m.mat[3] * m.mat[6]));
        result.mat[4] = id * -(+m.mat[4] * (m.mat[10] * m.mat[15] - m.mat[11] * m.mat[14]) - m.mat[8] * (m.mat[6] * m.mat[15] - m.mat[7] * m.mat[14]) + m.mat[12] * (m.mat[6] * m.mat[11] - m.mat[7] * m.mat[10]));
        result.mat[5] = id * (+m.mat[0] * (m.mat[10] * m.mat[15] - m.mat[11] * m.mat[14]) - m.mat[8] * (m.mat[2] * m.mat[15] - m.mat[3] * m.mat[14]) + m.mat[12] * (m.mat[2] * m.mat[11] - m.mat[3] * m.mat[10]));
        result.mat[6] = id * -(+m.mat[0] * (m.mat[6] * m.mat[15] - m.mat[7] * m.mat[14]) - m.mat[4] * (m.mat[2] * m.mat[15] - m.mat[3] * m.mat[14]) + m.mat[12] * (m.mat[2] * m.mat[7] - m.mat[3] * m.mat[6]));
        result.mat[7] = id * (+m.mat[0] * (m.mat[6] * m.mat[11] - m.mat[7] * m.mat[10]) - m.mat[4] * (m.mat[2] * m.mat[11] - m.mat[3] * m.mat[10]) + m.mat[8] * (m.mat[2] * m.mat[7] - m.mat[3] * m.mat[6]));
        result.mat[8] = id * (+m.mat[4] * (m.mat[9] * m.mat[15] - m.mat[11] * m.mat[13]) - m.mat[8] * (m.mat[5] * m.mat[15] - m.mat[7] * m.mat[13]) + m.mat[12] * (m.mat[5] * m.mat[11] - m.mat[7] * m.mat[9]));
        result.mat[9] = id * -(+m.mat[0] * (m.mat[9] * m.mat[15] - m.mat[11] * m.mat[13]) - m.mat[8] * (m.mat[1] * m.mat[15] - m.mat[3] * m.mat[13]) + m.mat[12] * (m.mat[1] * m.mat[11] - m.mat[3] * m.mat[9]));
        result.mat[10] = id * (+m.mat[0] * (m.mat[5] * m.mat[15] - m.mat[7] * m.mat[13]) - m.mat[4] * (m.mat[1] * m.mat[15] - m.mat[3] * m.mat[13]) + m.mat[12] * (m.mat[1] * m.mat[7] - m.mat[3] * m.mat[5]));
        result.mat[11] = id * -(+m.mat[0] * (m.mat[5] * m.mat[11] - m.mat[7] * m.mat[9]) - m.mat[4] * (m.mat[1] * m.mat[11] - m.mat[3] * m.mat[9]) + m.mat[8] * (m.mat[1] * m.mat[7] - m.mat[3] * m.mat[5]));
        result.mat[12] = id * -(+m.mat[4] * (m.mat[9] * m.mat[14] - m.mat[10] * m.mat[13]) - m.mat[8] * (m.mat[5] * m.mat[14] - m.mat[6] * m.mat[13]) + m.mat[12] * (m.mat[5] * m.mat[10] - m.mat[6] * m.mat[9]));
        result.mat[13] = id * (+m.mat[0] * (m.mat[9] * m.mat[14] - m.mat[10] * m.mat[13]) - m.mat[8] * (m.mat[1] * m.mat[14] - m.mat[2] * m.mat[13]) + m.mat[12] * (m.mat[1] * m.mat[10] - m.mat[2] * m.mat[9]));
        result.mat[14] = id * -(+m.mat[0] * (m.mat[5] * m.mat[14] - m.mat[6] * m.mat[13]) - m.mat[4] * (m.mat[1] * m.mat[14] - m.mat[2] * m.mat[13]) + m.mat[12] * (m.mat[1] * m.mat[6] - m.mat[2] * m.mat[5]));
        result.mat[15] = id * (+m.mat[0] * (m.mat[5] * m.mat[10] - m.mat[6] * m.mat[9]) - m.mat[4] * (m.mat[1] * m.mat[10] - m.mat[2] * m.mat[9]) + m.mat[8] * (m.mat[1] * m.mat[6] - m.mat[2] * m.mat[5]));
        return result;
    }
}
}
#endif

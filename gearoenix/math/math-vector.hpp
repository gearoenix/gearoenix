#ifndef GEAROEMIX_MATH_VECTOR_HPP
#define GEAROEMIX_MATH_VECTOR_HPP
namespace gearoenix {
namespace system {
    class File;
}
namespace math {
    template <class T>
    class Mat4x4;
    template <class T>
    class Mat3x3;

    template <typename T = float>
    struct Vec2D {
    private:
        T vec[2];

    public:
        Vec2D()
        {
            vec[0] = T(0);
            vec[1] = T(0);
        }
        Vec2D(T x, T y)
        {
            vec[0] = x;
            vec[1] = y;
        }
        Vec2D(T e)
        {
            vec[0] = e;
            vec[1] = e;
        }
        Vec2D(const Vec2D<T>& v)
        {
            vec[0] = v.vec[0];
            vec[1] = v.vec[1];
        }
        void operator=(const Vec2D<T>& v)
        {
            vec[0] = v.vec[0];
            vec[1] = v.vec[1];
        }
        Vec2D<T> operator+(const Vec2D<T>& a) const
        {
            return Vec2D<T>(vec[0] + a.vec[0], vec[1] + a.vec[1]);
        }
        Vec2D<T> operator-(const Vec2D<T>& a) const
        {
            return Vec2D<T>(vec[0] - a.vec[0], vec[1] - a.vec[1]);
        }
        Vec2D<T> operator*(const Vec2D<T>& a) const
        {
            return Vec2D<T>(vec[0] * a.vec[0], vec[1] * a.vec[1]);
        }
        Vec2D<T> operator*(const T& a) const
        {
            return Vec2D<T>(vec[0] * a, vec[1] * a);
        }
        Vec2D<T> operator/(const Vec2D<T>& a) const
        {
            return Vec2D<T>(vec[0] / a.vec[0], vec[1] / a.vec[1]);
        }
        void operator+=(const Vec2D<T>& a)
        {
            vec[0] += a.vec[0];
            vec[1] += a.vec[1];
        }
        void operator-=(const Vec2D<T>& a)
        {
            vec[0] -= a.vec[0];
            vec[1] -= a.vec[1];
        }
        void operator*=(const Vec2D<T>& a)
        {
            vec[0] *= a.vec[0];
            vec[1] *= a.vec[1];
        }
        void operator/=(const Vec2D<T>& a)
        {
            vec[0] /= a.vec[0];
            vec[1] /= a.vec[1];
        }
        Vec2D<T> operator-() const
        {
            return Vec2D<T>(-vec[0], -vec[1]);
        }
        T length()
        {
            return T(sqrt(vec[0] * vec[0] + vec[1] * vec[1]));
        }
        T square_distance(const Vec2D& a) const
        {
            T t = a.vec[0] - vec[0];
            t *= t;
            T d = t;
            t = a.vec[1] - vec[1];
            t *= t;
            d += t;
            return d;
        }
        void read(system::File* f)
        {
            f->read(vec[0]);
            f->read(vec[1]);
        }
        void print()
        {
            LOGI("Vec2D(" << vec[0] << ", " << vec[1] << ")");
        }
    };
    template <typename T = float>
    struct Vec3D {
    private:
        T vec[3];

    public:
        Vec3D()
        {
        }
        Vec3D(T x, T y, T z)
        {
            vec[0] = x;
            vec[1] = y;
            vec[2] = z;
        }
        Vec3D(T e)
        {
            vec[0] = e;
            vec[1] = e;
            vec[2] = e;
        }
        Vec3D(const Vec3D<T>& v)
        {
            vec[0] = v.vec[0];
            vec[1] = v.vec[1];
            vec[2] = v.vec[2];
        }
        void operator=(const Vec3D<T>& v)
        {
            vec[0] = v.vec[0];
            vec[1] = v.vec[1];
            vec[2] = v.vec[2];
        }
        Vec3D<T> operator+(const Vec3D<T>& a) const
        {
            return Vec3D<T>(vec[0] + a.vec[0], vec[1] + a.vec[1], vec[2] + a.vec[2]);
        }
        Vec3D<T> operator-(const Vec3D<T>& a) const
        {
            return Vec3D<T>(vec[0] - a.vec[0], vec[1] - a.vec[1], vec[2] - a.vec[2]);
        }
        Vec3D<T> operator*(const Vec3D<T>& a) const
        {
            return Vec3D<T>(vec[0] * a.vec[0], vec[1] * a.vec[1], vec[2] * a.vec[2]);
        }
        Vec3D<T> operator*(const T& a) const
        {
            return Vec3D<T>(vec[0] * a, vec[1] * a, vec[2] * a);
        }
        Vec3D<T> operator/(const Vec3D<T>& a) const
        {
            return Vec3D<T>(vec[0] / a.vec[0], vec[1] / a.vec[1], vec[2] / a.vec[2]);
        }
        void operator+=(const Vec3D<T>& a)
        {
            vec[0] += a.vec[0];
            vec[1] += a.vec[1];
            vec[2] += a.vec[2];
        }
        void operator-=(const Vec3D<T>& a)
        {
            vec[0] -= a.vec[0];
            vec[1] -= a.vec[1];
            vec[2] -= a.vec[2];
        }
        void operator*=(const Vec3D<T>& a)
        {
            vec[0] *= a.vec[0];
            vec[1] *= a.vec[1];
            vec[2] *= a.vec[2];
        }
        void operator/=(const Vec3D<T>& a)
        {
            vec[0] /= a.vec[0];
            vec[1] /= a.vec[1];
            vec[2] /= a.vec[2];
        }
        Vec3D<T> operator-() const
        {
            return Vec3D<T>(-vec[0], -vec[1], -vec[2]);
        }
        T length()
        {
            return T(sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]));
        }
        T square_distance(const Vec3D& a) const
        {
            T t = a.vec[0] - vec[0];
            t *= t;
            T d = t;
            t = a.vec[1] - vec[1];
            t *= t;
            d += t;
            t = a.vec[2] - vec[2];
            t *= t;
            d += t;
            return d;
        }
        T abs()
        {
            return std::abs(vec[0]) + std::abs(vec[1]) + std::abs(vec[2]);
        }
        void read(system::File* f)
        {
            f->read(vec[0]);
            f->read(vec[1]);
            f->read(vec[2]);
        }
        void print()
        {
            LOGI("Vec2D(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")");
        }
    };
    template <typename T = float>
    struct Vec4D {
    private:
        T vec[4];

    public:
        Vec4D(T x, T y, T z, T w)
        {
            vec[0] = x;
            vec[1] = y;
            vec[2] = z;
            vec[3] = w;
        }
        Vec4D(T e)
        {
            vec[0] = e;
            vec[1] = e;
            vec[2] = e;
            vec[3] = e;
        }
        T dot(const Vec4D& another) const
        {
            return (vec[0] * another.vec[0]) + (vec[1] * another.vec[1]) + (vec[2] * another.vec[2]) + (vec[3] * another.vec[3]);
        }
        static T dot(const Vec4D& a, const Vec4D& b)
        {
            return (b.vec[0] * a.vec[0]) + (b.vec[1] * a.vec[1]) + (b.vec[2] * a.vec[2]) + (b.vec[3] * a.vec[3]);
        }
        Vec4D<T> cross(const Vec4D<T>& a) const
        {
            return Vec4D<T>((vec[1] * a.vec[2]) - (vec[2] * a.vec[1]),
                (vec[2] * a.vec[0]) - (vec[0] * a.vec[2]),
                (vec[0] * a.vec[1]) - (vec[1] * a.vec[0]),
                vec[3] * a.vec[3]);
        }
        static Vec4D<T> cross(const Vec4D<T>& a, const Vec4D<T>& b)
        {
            return Vec4D<T>((a.vec[1] * b.vec[2]) - (a.vec[2] * b.vec[1]),
                (a.vec[2] * b.vec[0]) - (a.vec[0] * b.vec[2]),
                (a.vec[0] * b.vec[1]) - (a.vec[1] * b.vec[0]),
                a.vec[3] * b.vec[3]);
        }
        void normalize()
        {
            T len = (T)sqrt((double)(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]));
            vec[0] /= len;
            vec[1] /= len;
            vec[2] /= len;
            vec[3] /= len;
        }
        static Vec4D<T> normalize(const Vec4D<T>& a)
        {
            T len = (T)sqrt((double)(a.vec[0] * a.vec[0] + a.vec[1] * a.vec[1] + a.vec[2] * a.vec[2] + a.vec[3] * a.vec[3]));
            return Vec4D<T>(a.vec[0] / len, a.vec[1] / len, a.vec[2] / len, a.vec[3] / len);
        }
    };
}
}
#endif

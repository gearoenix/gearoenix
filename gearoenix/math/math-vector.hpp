#ifndef GEAROEMIX_MATH_VECTOR_HPP
#define GEAROEMIX_MATH_VECTOR_HPP
#include "../core/cr-types.hpp"
namespace gearoenix {
namespace system {
    class File;
}
namespace math {
    struct Mat4x4;
    struct Mat3x3;
    struct Vec2 {
    private:
        core::Real vec[2];

    public:
        Vec2();
        Vec2(core::Real e);
        Vec2(core::Real x, core::Real y);
        Vec2(const Vec2& v);
        Vec2 operator+(const Vec2& a) const;
        Vec2 operator-(const Vec2& a) const;
        Vec2 operator*(const Vec2& a) const;
        Vec2 operator*(const core::Real& a) const;
        Vec2 operator/(const Vec2& a) const;
        Vec2 operator-() const;
        void operator=(const Vec2& v);
        void operator+=(const Vec2& a);
        void operator-=(const Vec2& a);
        void operator*=(const Vec2& a);
        void operator/=(const Vec2& a);
        const core::Real& operator[](const unsigned int i) const;
        core::Real& operator[](const unsigned int i);
        core::Real dot(const Vec2& o) const;
        core::Real length() const;
        core::Real square_distance(const Vec2& a) const;
        Vec2 normalized() const;
        void normalize();
        void read(system::File* f);
        void print() const;
    };

    struct Vec3 {
    private:
        core::Real vec[3];

    public:
        static const Vec3 X;
        static const Vec3 Y;
        static const Vec3 Z;
        Vec3();
        Vec3(core::Real e);
        Vec3(core::Real x, core::Real y, core::Real z);
        Vec3(const Vec3& v);
        Vec3 operator-() const;
        Vec3 operator+(const Vec3& a) const;
        Vec3 operator-(const Vec3& a) const;
        Vec3 operator*(const Vec3& a) const;
        Vec3 operator*(const core::Real& a) const;
        Vec3 operator/(const Vec3& a) const;
        void operator=(const Vec3& v);
        void operator+=(const Vec3& a);
        void operator-=(const Vec3& a);
        void operator*=(const Vec3& a);
        void operator/=(const Vec3& a);
        const core::Real& operator[](const unsigned int i) const;
        core::Real& operator[](const unsigned int i);
        core::Real* data();
        core::Real length() const;
        core::Real square_length() const;
        core::Real square_distance(const Vec3& a) const;
        core::Real abs() const;
        core::Real dot(const Vec3& o) const;
        Vec3 cross(const Vec3& o) const;
        Vec3 normalized() const;
        void normalize();
        void read(system::File* f);
        void print() const;
    };

    struct Vec4 {
    private:
        core::Real vec[4];

    public:
        Vec4();
        Vec4(core::Real e);
        Vec4(core::Real x, core::Real y, core::Real z, core::Real w);
        Vec4(const Vec3& v, core::Real w);
        Vec3 xyz() const;
        void xyz(const Vec3& v);
        core::Real w() const;
        void w(core::Real);
        const core::Real& operator[](const unsigned int i) const;
        core::Real& operator[](const unsigned int i);
        core::Real dot(const Vec4& o) const;
        Vec4 cross(const Vec4& o) const;
        Vec4 normalized() const;
        void normalize();
        void read(system::File* f);
        const core::Real* data() const;
    };
}
}
#endif

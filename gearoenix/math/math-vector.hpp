#ifndef GEAROENIX_MATH_VECTOR_HPP
#define GEAROENIX_MATH_VECTOR_HPP
#include "../core/cr-types.hpp"
#include <ostream>

namespace gearoenix {
namespace system::stream {
    class Stream;
}
namespace math {
    struct Mat4x4;
    struct Vec2 {
    private:
        core::Real vec[2];

    public:
        Vec2() noexcept;
        Vec2(core::Real x, core::Real y) noexcept;
        Vec2(const Vec2& v) noexcept;
        explicit Vec2(core::Real e) noexcept;
        Vec2 operator+(const Vec2& a) const noexcept;
        Vec2 operator-(const Vec2& a) const noexcept;
        Vec2 operator*(const Vec2& a) const noexcept;
        Vec2 operator*(const core::Real& a) const noexcept;
        Vec2 operator/(const Vec2& a) const noexcept;
        Vec2 operator-() const noexcept;
        Vec2& operator=(const Vec2& v) noexcept;
        void operator+=(const Vec2& a) noexcept;
        void operator-=(const Vec2& a) noexcept;
        void operator*=(const Vec2& a) noexcept;
        void operator*=(core::Real a) noexcept;
        void operator/=(const Vec2& a) noexcept;
        const core::Real& operator[](unsigned int i) const noexcept;
        core::Real& operator[](unsigned int i) noexcept;
        const core::Real& operator[](int i) const noexcept;
        core::Real& operator[](int i) noexcept;
        bool operator<(const Vec2& o) const noexcept;
        bool operator==(const Vec2& o) const noexcept;
        bool operator>(const Vec2& o) const noexcept;
        [[nodiscard]] core::Real dot(const Vec2& o) const noexcept;
        [[nodiscard]] core::Real cross(const Vec2& o) const noexcept;
        [[nodiscard]] core::Real length() const noexcept;
        [[nodiscard]] core::Real square_length() const noexcept;
        [[nodiscard]] core::Real distance(const Vec2& a) const noexcept;
        [[nodiscard]] core::Real square_distance(const Vec2& a) const noexcept;
        [[nodiscard]] Vec2 normalized() const noexcept;
        void normalize() noexcept;
        void read(system::stream::Stream* f) noexcept;
        static bool intersect(const Vec2& s11, const Vec2& s12, const Vec2& s21, const Vec2& s22, Vec2& i) noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Vec2& v) noexcept
        {
            os << "Vec2 {" << v[0] << ", " << v[1] << "}";
            return os;
        }
    };

    struct Vec3 {
    private:
        core::Real vec[3];

    public:
        static const Vec3 X;
        static const Vec3 Y;
        static const Vec3 Z;
        Vec3() noexcept;
        Vec3(const math::Vec2& o, core::Real e) noexcept;
        constexpr Vec3(const core::Real x, const core::Real y, const core::Real z) noexcept
            : vec { x, y, z }
        {
        }
        constexpr Vec3(const Vec3& v) noexcept
            : vec { v.vec[0], v.vec[1], v.vec[2] }
        {
        }
        explicit Vec3(core::Real e) noexcept;
        Vec3 operator-() const noexcept;
        Vec3 operator+(const Vec3& a) const noexcept;
        Vec3 operator+(core::Real a) const noexcept;
        Vec3 operator-(const Vec3& a) const noexcept;
        Vec3 operator-(core::Real a) const noexcept;
        Vec3 operator*(const Vec3& a) const noexcept;
        Vec3 operator*(core::Real a) const noexcept;
        Vec3 operator/(const Vec3& a) const noexcept;
        Vec3 operator/(core::Real a) const noexcept;
        Vec3& operator=(const Vec3& v) noexcept;
        void operator+=(const Vec3& a) noexcept;
        void operator-=(const Vec3& a) noexcept;
        void operator*=(const Vec3& a) noexcept;
        void operator/=(const Vec3& a) noexcept;
        void operator*=(core::Real a) noexcept;
        void operator/=(core::Real a) noexcept;
        const core::Real& operator[](unsigned int i) const noexcept;
        core::Real& operator[](unsigned int i) noexcept;
        const core::Real& operator[](int i) const noexcept;
        core::Real& operator[](int i) noexcept;
        bool operator<(const Vec3& o) const noexcept;
        bool operator==(const Vec3& o) const noexcept;
        bool operator>(const Vec3& o) const noexcept;
        [[nodiscard]] Vec2 xy() const noexcept;
        [[nodiscard]] const core::Real* data() const noexcept;
        [[nodiscard]] core::Real length() const noexcept;
        [[nodiscard]] core::Real square_length() const noexcept;
        [[nodiscard]] core::Real abs_length() const noexcept;
        [[nodiscard]] core::Real square_distance(const Vec3& a) const noexcept;
        [[nodiscard]] core::Real abs_distance(const Vec3& a) const noexcept;
        [[nodiscard]] Vec3 abs() const noexcept;
        [[nodiscard]] core::Real dot(const Vec3& o) const noexcept;
        [[nodiscard]] Vec3 cross(const Vec3& o) const noexcept;
        [[nodiscard]] Vec3 minimum(const Vec3& o) const noexcept;
        [[nodiscard]] core::Real minimum() const noexcept;
        [[nodiscard]] Vec3 maximum(const Vec3& o) const noexcept;
        [[nodiscard]] core::Real maximum() const noexcept;
        [[nodiscard]] Vec3 normalized() const noexcept;
        void normalize() noexcept;
        void read(system::stream::Stream* f) noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Vec3& v) noexcept
        {
            os << "Vec3 {" << v[0] << ", " << v[1] << ", " << v[2] << "}";
            return os;
        }
    };

    struct Vec4 {
    private:
        core::Real vec[4];

    public:
        static const Vec4 X;
        static const Vec4 Y;
        static const Vec4 Z;

        Vec4() noexcept;
        Vec4(core::Real x, core::Real y, core::Real z, core::Real w) noexcept;
        Vec4(const Vec2& v, core::Real z, core::Real w) noexcept;
        Vec4(const Vec3& v, core::Real w) noexcept;
        explicit Vec4(core::Real e) noexcept;
        [[nodiscard]] Vec3 xyz() const noexcept;
        void xyz(const Vec3& v) noexcept;
        [[nodiscard]] core::Real w() const noexcept;
        void w(core::Real) noexcept;
        const core::Real& operator[](unsigned int i) const noexcept;
        core::Real& operator[](unsigned int i) noexcept;
        const core::Real& operator[](int i) const noexcept;
        core::Real& operator[](int i) noexcept;
        bool operator<(const Vec4& o) const noexcept;
        bool operator==(const Vec4& o) const noexcept;
        bool operator>(const Vec4& o) const noexcept;
        Vec4 operator*(core::Real a) const noexcept;
        Vec4 operator-(const Vec4& a) const noexcept;
        [[nodiscard]] core::Real length() const noexcept;
        [[nodiscard]] core::Real square_length() const noexcept;
        [[nodiscard]] core::Real dot(const Vec4& o) const noexcept;
        [[nodiscard]] Vec4 cross(const Vec4& o) const noexcept;
        [[nodiscard]] Vec4 normalized() const noexcept;
        void normalize() noexcept;
        void read(system::stream::Stream* f) noexcept;
        [[nodiscard]] const core::Real* data() const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Vec4& v) noexcept
        {
            os << "Vec4 {" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "}";
            return os;
        }
    };
}
}
#endif

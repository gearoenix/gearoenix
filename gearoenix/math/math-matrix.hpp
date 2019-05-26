#ifndef GEAROENIX_MATH_MATRIX_HPP
#define GEAROENIX_MATH_MATRIX_HPP
#include "../core/cr-types.hpp"
#include <memory>

namespace gearoenix {
namespace system::stream {
    class Stream;
}
namespace math {
    struct Vec2;
    struct Vec3;
    struct Vec4;
    // be careful this is column major
    struct Mat2x2 {
    private:
        core::Real mat[4];

    public:
        Mat2x2();
        explicit Mat2x2(core::Real dia_e);
        Mat2x2(core::Real e0, core::Real e1, core::Real e2, core::Real e3);
        bool invert();
        Vec2 operator*(const Vec2& v) const;
    };
    struct Mat4x4 {
    private:
        core::Real mat[16];

    public:
        explicit Mat4x4(core::Real e);
        Mat4x4();
        Mat4x4(
            core::Real e0, core::Real e1, core::Real e2, core::Real e3,
            core::Real e4, core::Real e5, core::Real e6, core::Real e7,
            core::Real e8, core::Real e9, core::Real e10, core::Real e11,
            core::Real e12, core::Real e13, core::Real e14, core::Real e15);
        explicit Mat4x4(const std::shared_ptr<system::stream::Stream>& f);
        Mat4x4(const Mat4x4& m);
        Vec3 operator*(const Vec3& v) const;
        Vec4 operator*(const Vec4& v) const;
        Mat4x4 operator*(const Mat4x4& m) const;
        Mat4x4& operator=(const Mat4x4& m);
        void operator*=(const Mat4x4& m);
        const core::Real& operator[](unsigned int i) const;
        core::Real& operator[](unsigned int i);
        /// It does not change location
        void local_scale(const core::Real& s);
        /// It does not change location
        void local_scale(const core::Real& a, const core::Real& b, const core::Real& c);
        /// It changes location
        void local_scale(const core::Real& a, const core::Real& b, const core::Real& c, const core::Real& d);
        /// It does not change location
        void local_scale(const Vec3& s);
        /// It changes location
        void local_scale(const Vec4& s);
        /// It does not change location
        void global_scale(const core::Real& s);
        /// It does not change location
        void global_scale(const core::Real& a, const core::Real& b, const core::Real& c);
        /// It changes location
        void global_scale(const core::Real& a, const core::Real& b, const core::Real& c, const core::Real& d);
        /// It does not change location
        void global_scale(const Vec3& s);
        /// It changes location
        void global_scale(const Vec4& s);
        void translate(const Vec3& v);
        void set_location_zero();
        void set_location(const Vec3& location);
        void get_location(Vec3& location) const;
        void inverse() noexcept;
        void transpose() noexcept;
        void read(const std::shared_ptr<system::stream::Stream>& f);
        core::Real determinant() const;
        Mat4x4 inverted() const;
        Mat4x4 transposed() const;
        const core::Real* data() const;
        static Mat4x4 look_at(const Vec3& position, const Vec3& target, const Vec3& up);
        static Mat4x4 rotation(const Vec3& v, core::Real degree);
        static Mat4x4 translator(const Vec3& v);
        static Mat4x4 orthographic(core::Real width, core::Real height, core::Real near, core::Real far);
        static Mat4x4 perspective(core::Real width, core::Real height, core::Real near, core::Real far);
    };
}
}
#endif

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
        Mat2x2() noexcept;
        explicit Mat2x2(core::Real dia_e) noexcept;
        Mat2x2(core::Real e0, core::Real e1, core::Real e2, core::Real e3) noexcept;
        bool invert() noexcept;
        Vec2 operator*(const Vec2& v) const noexcept;
    };
    struct Mat4x4 {
    private:
        core::Real mat[16];

    public:
        explicit Mat4x4(core::Real e) noexcept;
        Mat4x4() noexcept;
        Mat4x4(
            core::Real e0, core::Real e1, core::Real e2, core::Real e3,
            core::Real e4, core::Real e5, core::Real e6, core::Real e7,
            core::Real e8, core::Real e9, core::Real e10, core::Real e11,
            core::Real e12, core::Real e13, core::Real e14, core::Real e15) noexcept;
        explicit Mat4x4(system::stream::Stream* f) noexcept;
        Mat4x4(const Mat4x4& m) noexcept;
        Vec4 operator*(const Vec4& v) const noexcept;
        Mat4x4 operator*(const Mat4x4& m) const noexcept;
        Mat4x4& operator=(const Mat4x4& m) noexcept;
        void operator*=(const Mat4x4& m) noexcept;
        const core::Real& operator[](unsigned int i) const noexcept;
        core::Real& operator[](unsigned int i) noexcept;
        /// It does not change location
        void local_scale(core::Real s) noexcept;
        /// It does not change location
        void local_x_scale(core::Real s) noexcept;
        /// It does not change location
        void local_y_scale(core::Real s) noexcept;
        /// It does not change location
        void local_z_scale(core::Real s) noexcept;
        /// It does not change location
        void local_w_scale(core::Real s) noexcept;
        /// It does not change location
        void local_scale(core::Real a, core::Real b, core::Real c) noexcept;
        /// It changes location
        void local_scale(core::Real a, core::Real b, core::Real c, core::Real d) noexcept;
        /// It does not change location
        void local_scale(const Vec3& s) noexcept;
        /// It changes location
        void local_scale(const Vec4& s) noexcept;
        /// It changes location
        void global_scale(core::Real s) noexcept;
        /// It changes location
        void global_scale(core::Real a, core::Real b, core::Real c) noexcept;
        /// It changes location
        void global_scale(core::Real a, core::Real b, core::Real c, core::Real d) noexcept;
        /// It changes location
        void global_scale(const Vec3& s) noexcept;
        /// It changes location
        void global_scale(const Vec4& s) noexcept;
        void translate(const Vec3& v) noexcept;
        void set_location_zero() noexcept;
        void set_location(const Vec3& location) noexcept;
        void get_location(Vec3& location) const noexcept;
        void inverse() noexcept;
        void transpose() noexcept;
        void read(system::stream::Stream* f) noexcept;
        core::Real determinant() const noexcept;
        Mat4x4 inverted() const noexcept;
        Mat4x4 transposed() const noexcept;
        const core::Real* data() const noexcept;
        Vec3 project(const Vec3& v) const noexcept;
        static Mat4x4 look_at(const Vec3& position, const Vec3& target, const Vec3& up) noexcept;
        static Mat4x4 rotation(const Vec3& v, core::Real degree) noexcept;
        static Mat4x4 translator(const Vec3& v) noexcept;
        static Mat4x4 orthographic(core::Real width, core::Real height, core::Real near, core::Real far) noexcept;
        static Mat4x4 perspective(core::Real width, core::Real height, core::Real near, core::Real far) noexcept;
    };
}
}
#endif

#include "math-vector.hpp"
#include "../core/cr-build-configuration.hpp"
#include "../system/sys-file.hpp"
#include "../system/sys-log.hpp"
#include <cmath>

gearoenix::math::Vec2::Vec2()
{
    vec[0] = core::Real(0);
    vec[1] = core::Real(0);
}

gearoenix::math::Vec2::Vec2(core::Real x, core::Real y)
{
    vec[0] = x;
    vec[1] = y;
}

gearoenix::math::Vec2::Vec2(core::Real e)
{
    vec[0] = e;
    vec[1] = e;
}

gearoenix::math::Vec2::Vec2(const Vec2& v)
{
    vec[0] = v.vec[0];
    vec[1] = v.vec[1];
}

void gearoenix::math::Vec2::operator=(const Vec2& v)
{
    vec[0] = v.vec[0];
    vec[1] = v.vec[1];
}

gearoenix::math::Vec2 gearoenix::math::Vec2::operator+(const Vec2& a) const
{
    return Vec2(vec[0] + a.vec[0], vec[1] + a.vec[1]);
}

gearoenix::math::Vec2 gearoenix::math::Vec2::operator-(const Vec2& a) const
{
    return Vec2(vec[0] - a.vec[0], vec[1] - a.vec[1]);
}

gearoenix::math::Vec2 gearoenix::math::Vec2::operator*(const Vec2& a) const
{
    return Vec2(vec[0] * a.vec[0], vec[1] * a.vec[1]);
}

gearoenix::math::Vec2 gearoenix::math::Vec2::operator*(const core::Real& a) const
{
    return Vec2(vec[0] * a, vec[1] * a);
}

gearoenix::math::Vec2 gearoenix::math::Vec2::operator/(const Vec2& a) const
{
    return Vec2(vec[0] / a.vec[0], vec[1] / a.vec[1]);
}

void gearoenix::math::Vec2::operator+=(const Vec2& a)
{
    vec[0] += a.vec[0];
    vec[1] += a.vec[1];
}

void gearoenix::math::Vec2::operator-=(const Vec2& a)
{
    vec[0] -= a.vec[0];
    vec[1] -= a.vec[1];
}

void gearoenix::math::Vec2::operator*=(const Vec2& a)
{
    vec[0] *= a.vec[0];
    vec[1] *= a.vec[1];
}

void gearoenix::math::Vec2::operator/=(const Vec2& a)
{
    vec[0] /= a.vec[0];
    vec[1] /= a.vec[1];
}

const gearoenix::core::Real& gearoenix::math::Vec2::operator[](const unsigned int i) const
{
#ifdef DEBUG_MODE
    if (i > 1)
        LOGF("Out of range access");
#endif
    return vec[i];
}

gearoenix::core::Real& gearoenix::math::Vec2::operator[](const unsigned int i)
{
#ifdef DEBUG_MODE
    if (i > 1)
        LOGF("Out of range access");
#endif
    return vec[i];
}

gearoenix::core::Real gearoenix::math::Vec2::dot(const Vec2& o) const
{
    return (vec[0] * o.vec[0]) + (vec[1] * o.vec[1]);
}

gearoenix::math::Vec2 gearoenix::math::Vec2::operator-() const
{
    return Vec2(-vec[0], -vec[1]);
}

gearoenix::core::Real gearoenix::math::Vec2::length() const
{
    return core::Real(sqrt(vec[0] * vec[0] + vec[1] * vec[1]));
}

gearoenix::core::Real gearoenix::math::Vec2::square_distance(const Vec2& a) const
{
    core::Real t = a.vec[0] - vec[0];
    t *= t;
    core::Real d = t;
    t = a.vec[1] - vec[1];
    t *= t;
    d += t;
    return d;
}

void gearoenix::math::Vec2::read(system::File* f)
{
    f->read(vec[0]);
    f->read(vec[1]);
}

void gearoenix::math::Vec2::print() const
{
    LOGI("Vec2(" << vec[0] << ", " << vec[1] << ")");
}

const gearoenix::math::Vec3 gearoenix::math::Vec3::X(1.0f, 0.0f, 0.0f);
const gearoenix::math::Vec3 gearoenix::math::Vec3::Y(0.0f, 1.0f, 0.0f);
const gearoenix::math::Vec3 gearoenix::math::Vec3::Z(0.0f, 0.0f, 1.0f);

gearoenix::math::Vec3::Vec3()
{
    vec[0] = 0;
    vec[1] = 0;
    vec[2] = 0;
}

gearoenix::math::Vec3::Vec3(core::Real x, core::Real y, core::Real z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}

gearoenix::math::Vec3::Vec3(core::Real e)
{
    vec[0] = e;
    vec[1] = e;
    vec[2] = e;
}

gearoenix::math::Vec3::Vec3(const Vec3& v)
{
    vec[0] = v.vec[0];
    vec[1] = v.vec[1];
    vec[2] = v.vec[2];
}

void gearoenix::math::Vec3::operator=(const Vec3& v)
{
    vec[0] = v.vec[0];
    vec[1] = v.vec[1];
    vec[2] = v.vec[2];
}

gearoenix::math::Vec3 gearoenix::math::Vec3::operator+(const Vec3& a) const
{
    return Vec3(vec[0] + a.vec[0], vec[1] + a.vec[1], vec[2] + a.vec[2]);
}

gearoenix::math::Vec3 gearoenix::math::Vec3::operator-(const Vec3& a) const
{
    return Vec3(vec[0] - a.vec[0], vec[1] - a.vec[1], vec[2] - a.vec[2]);
}

gearoenix::math::Vec3 gearoenix::math::Vec3::operator*(const Vec3& a) const
{
    return Vec3(vec[0] * a.vec[0], vec[1] * a.vec[1], vec[2] * a.vec[2]);
}

gearoenix::math::Vec3 gearoenix::math::Vec3::operator*(const core::Real& a) const
{
    return Vec3(vec[0] * a, vec[1] * a, vec[2] * a);
}

gearoenix::math::Vec3 gearoenix::math::Vec3::operator/(const Vec3& a) const
{
    return Vec3(vec[0] / a.vec[0], vec[1] / a.vec[1], vec[2] / a.vec[2]);
}

void gearoenix::math::Vec3::operator+=(const Vec3& a)
{
    vec[0] += a.vec[0];
    vec[1] += a.vec[1];
    vec[2] += a.vec[2];
}

void gearoenix::math::Vec3::operator-=(const Vec3& a)
{
    vec[0] -= a.vec[0];
    vec[1] -= a.vec[1];
    vec[2] -= a.vec[2];
}

void gearoenix::math::Vec3::operator*=(const Vec3& a)
{
    vec[0] *= a.vec[0];
    vec[1] *= a.vec[1];
    vec[2] *= a.vec[2];
}

void gearoenix::math::Vec3::operator/=(const Vec3& a)
{
    vec[0] /= a.vec[0];
    vec[1] /= a.vec[1];
    vec[2] /= a.vec[2];
}

const gearoenix::core::Real& gearoenix::math::Vec3::operator[](const unsigned int i) const
{
#ifdef DEBUG_MODE
    if (i > 2) {
        LOGF("Out of range access");
    }
#endif
    return vec[i];
}

gearoenix::core::Real& gearoenix::math::Vec3::operator[](const unsigned int i)
{
#ifdef DEBUG_MODE
    if (i > 2) {
        LOGF("Out of range access");
    }
#endif
    return vec[i];
}

gearoenix::math::Vec3 gearoenix::math::Vec3::operator-() const
{
    return Vec3(-vec[0], -vec[1], -vec[2]);
}

gearoenix::core::Real gearoenix::math::Vec3::length() const
{
    return core::Real(sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]));
}

gearoenix::core::Real gearoenix::math::Vec3::square_length() const
{
    return vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];
}

gearoenix::core::Real gearoenix::math::Vec3::square_distance(const Vec3& a) const
{
    core::Real t = a.vec[0] - vec[0];
    t *= t;
    core::Real d = t;
    t = a.vec[1] - vec[1];
    t *= t;
    d += t;
    t = a.vec[2] - vec[2];
    t *= t;
    d += t;
    return d;
}

gearoenix::core::Real gearoenix::math::Vec3::abs() const
{
    return std::abs(vec[0]) + std::abs(vec[1]) + std::abs(vec[2]);
}

gearoenix::core::Real gearoenix::math::Vec3::dot(const Vec3& o) const
{
    return (vec[0] * o.vec[0]) + (vec[1] * o.vec[1]) + (vec[2] * o.vec[2]);
}

gearoenix::math::Vec3 gearoenix::math::Vec3::cross(const Vec3& o) const
{
    return Vec3((vec[1] * o.vec[2]) - (vec[2] * o.vec[1]),
        (vec[2] * o.vec[0]) - (vec[0] * o.vec[2]),
        (vec[0] * o.vec[1]) - (vec[1] * o.vec[0]));
}

gearoenix::math::Vec3 gearoenix::math::Vec3::normalized() const
{
    core::Real len = core::Real(sqrt(double(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2])));
    return Vec3(vec[0] / len, vec[1] / len, vec[2] / len);
}

void gearoenix::math::Vec3::read(system::File* f)
{
    f->read(vec[0]);
    f->read(vec[1]);
    f->read(vec[2]);
}
void gearoenix::math::Vec3::print() const
{
    LOGI("Vec2(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")");
}

gearoenix::math::Vec4::Vec4(core::Real x, core::Real y, core::Real z, core::Real w)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    vec[3] = w;
}

gearoenix::math::Vec4::Vec4(core::Real e)
{
    vec[0] = e;
    vec[1] = e;
    vec[2] = e;
    vec[3] = e;
}

const gearoenix::core::Real& gearoenix::math::Vec4::operator[](const unsigned int i) const
{
#ifdef DEBUG_MODE
    if (i > 3)
        LOGF("Out of range access");
#endif
    return vec[i];
}

gearoenix::core::Real& gearoenix::math::Vec4::operator[](const unsigned int i)
{
#ifdef DEBUG_MODE
    if (i > 3)
        LOGF("Out of range access");
#endif
    return vec[i];
}

gearoenix::core::Real gearoenix::math::Vec4::dot(const Vec4& o) const
{
    return (vec[0] * o.vec[0]) + (vec[1] * o.vec[1]) + (vec[2] * o.vec[2]) + (vec[3] * o.vec[3]);
}

gearoenix::math::Vec4 gearoenix::math::Vec4::cross(const Vec4& o) const
{
    return Vec4((vec[1] * o.vec[2]) - (vec[2] * o.vec[1]),
        (vec[2] * o.vec[0]) - (vec[0] * o.vec[2]),
        (vec[0] * o.vec[1]) - (vec[1] * o.vec[0]),
        vec[3] * o.vec[3]);
}

gearoenix::math::Vec4 gearoenix::math::Vec4::normalized() const
{
    core::Real len = (core::Real)sqrt((double)(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]));
    Vec4 r(vec[0] / len, vec[1] / len, vec[2] / len, vec[3] / len);
    return r;
}

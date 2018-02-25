#include "math-vector.hpp"
#include "../core/cr-build-configuration.hpp"
#include "../system/stream/sys-stm-stream.hpp"
#include "../system/sys-log.hpp"
#include "math-matrix.hpp"
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
        GXLOGF("Out of range access");
#endif
    return vec[i];
}

gearoenix::core::Real& gearoenix::math::Vec2::operator[](const unsigned int i)
{
#ifdef DEBUG_MODE
    if (i > 1)
        GXLOGF("Out of range access");
#endif
    return vec[i];
}

gearoenix::core::Real gearoenix::math::Vec2::dot(const Vec2& o) const
{
    return (vec[0] * o.vec[0]) + (vec[1] * o.vec[1]);
}

gearoenix::core::Real gearoenix::math::Vec2::cross(const Vec2& o) const
{
    return vec[0] * o.vec[1] - vec[1] * o.vec[0];
}

gearoenix::math::Vec2 gearoenix::math::Vec2::operator-() const
{
    return Vec2(-vec[0], -vec[1]);
}

gearoenix::core::Real gearoenix::math::Vec2::length() const
{
    return core::Real(sqrt(vec[0] * vec[0] + vec[1] * vec[1]));
}

gearoenix::core::Real gearoenix::math::Vec2::square_length() const
{
    return vec[0] * vec[0] + vec[1] * vec[1];
}

gearoenix::core::Real gearoenix::math::Vec2::distance(const Vec2& a) const
{
    core::Real t1 = vec[0] - a.vec[0];
    t1 *= t1;
    core::Real t2 = vec[1] - a.vec[1];
    t2 *= t2;
    t1 += t2;
    return std::sqrt(t1);
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

gearoenix::math::Vec2 gearoenix::math::Vec2::normalized() const
{
    core::Real l(sqrt(vec[0] * vec[0] + vec[1] * vec[1]));
    return Vec2(vec[0] / l, vec[1] / l);
}

void gearoenix::math::Vec2::normalize()
{
    core::Real l(sqrt(vec[0] * vec[0] + vec[1] * vec[1]));
    l = 1.0f / l;
    vec[0] *= l;
    vec[1] *= l;
}

void gearoenix::math::Vec2::read(system::stream::Stream* f)
{
    f->read(vec[0]);
    f->read(vec[1]);
}

void gearoenix::math::Vec2::print() const
{
    GXLOGI("Vec2(" << vec[0] << ", " << vec[1] << ")");
}

bool gearoenix::math::Vec2::intersect(const Vec2& s11, const Vec2& s12, const Vec2& s21, const Vec2& s22, Vec2& i)
{
    core::Real maxxs1, minxs1;
    if (s11.vec[0] > s12.vec[0]) {
        maxxs1 = s11.vec[0];
        minxs1 = s12.vec[0];
    } else {
        minxs1 = s11.vec[0];
        maxxs1 = s12.vec[0];
    }
    core::Real maxxs2, minxs2;
    if (s21.vec[0] > s22.vec[0]) {
        maxxs2 = s21.vec[0];
        minxs2 = s22.vec[0];
    } else {
        minxs2 = s21.vec[0];
        maxxs2 = s22.vec[0];
    }
    if (minxs1 > maxxs2 || minxs2 > maxxs1)
        return false;
    core::Real maxys1, minys1;
    if (s11.vec[1] > s12.vec[1]) {
        maxys1 = s11.vec[1];
        minys1 = s12.vec[1];
    } else {
        minys1 = s11.vec[1];
        maxys1 = s12.vec[1];
    }
    core::Real maxys2, minys2;
    if (s21.vec[1] > s22.vec[1]) {
        maxys2 = s21.vec[1];
        minys2 = s22.vec[1];
    } else {
        minys2 = s21.vec[1];
        maxys2 = s22.vec[1];
    }
    if (minys1 > maxys2 || minys2 > maxys1)
        return false;
    // s11 + s0 * v0 == s21 + s1 * v1
    // s21 - s11 = s0 * v0 - s1 * v1
    // d = | v0 v1 | * s
    Vec2 v0 = s12 - s11;
    Vec2 v1 = s22 - s21;
    Mat2x2 m(v0.vec[0], -v1.vec[0], v0.vec[1], -v1.vec[1]);
    if (!m.invert())
        return false;
    const Vec2 d = s21 - s11;
    const Vec2 s = m * d;
    if (s.vec[0] < 0.0f || s.vec[0] > 1.0f || s.vec[1] < 0.0f || s.vec[1] > 1.0f)
        return false;
    i = v0 * s.vec[0] + s11;
    return true;
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

gearoenix::math::Vec3::Vec3(const math::Vec2& o, core::Real e)
{
    vec[0] = o[0];
    vec[1] = o[1];
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
        GXLOGF("Out of range access");
    }
#endif
    return vec[i];
}

gearoenix::core::Real& gearoenix::math::Vec3::operator[](const unsigned int i)
{
#ifdef DEBUG_MODE
    if (i > 2) {
        GXLOGF("Out of range access");
    }
#endif
    return vec[i];
}

gearoenix::math::Vec2 gearoenix::math::Vec3::xy() const
{
    return Vec2(vec[0], vec[1]);
}

gearoenix::core::Real* gearoenix::math::Vec3::data()
{
    return vec;
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

void gearoenix::math::Vec3::normalize()
{
    core::Real l(sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]));
    l = 1.0f / l;
    vec[0] *= l;
    vec[1] *= l;
    vec[2] *= l;
}

void gearoenix::math::Vec3::read(system::stream::Stream* f)
{
    f->read(vec[0]);
    f->read(vec[1]);
    f->read(vec[2]);
}
void gearoenix::math::Vec3::print() const
{
    GXLOGI("Vec3(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")");
}

gearoenix::math::Vec4::Vec4()
{
    vec[0] = 0.0f;
    vec[1] = 0.0f;
    vec[2] = 0.0f;
    vec[3] = 1.0f;
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

gearoenix::math::Vec4::Vec4(const Vec3& v, core::Real w)
{
    vec[0] = v[0];
    vec[1] = v[1];
    vec[2] = v[2];
    vec[3] = w;
}

gearoenix::math::Vec3 gearoenix::math::Vec4::xyz() const
{
    return Vec3(vec[0], vec[1], vec[2]);
}

void gearoenix::math::Vec4::xyz(const Vec3& v)
{
    vec[0] = v[0];
    vec[1] = v[1];
    vec[2] = v[2];
    ;
}

gearoenix::core::Real gearoenix::math::Vec4::w() const
{
    return vec[3];
}

void gearoenix::math::Vec4::w(core::Real e)
{
    vec[3] = e;
}

const gearoenix::core::Real& gearoenix::math::Vec4::operator[](const unsigned int i) const
{
#ifdef DEBUG_MODE
    if (i > 3)
        GXLOGF("Out of range access");
#endif
    return vec[i];
}

gearoenix::core::Real& gearoenix::math::Vec4::operator[](const unsigned int i)
{
#ifdef DEBUG_MODE
    if (i > 3)
        GXLOGF("Out of range access");
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

void gearoenix::math::Vec4::read(system::stream::Stream* f)
{
    f->read(vec[0]);
    f->read(vec[1]);
    f->read(vec[2]);
    f->read(vec[3]);
}

const gearoenix::core::Real* gearoenix::math::Vec4::data() const
{
    return vec;
}

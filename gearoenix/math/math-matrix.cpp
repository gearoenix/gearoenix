#include "math-matrix.hpp"
#include "../system/sys-file.hpp"
#include "math-vector.hpp"
#include <cmath>

gearoenix::math::Mat4x4::Mat4x4(core::Real e)
{
    mat[0] = e;
    mat[1] = core::Real(0);
    mat[2] = core::Real(0);
    mat[3] = core::Real(0);
    mat[4] = core::Real(0);
    mat[5] = e;
    mat[6] = core::Real(0);
    mat[7] = core::Real(0);
    mat[8] = core::Real(0);
    mat[9] = core::Real(0);
    mat[10] = e;
    mat[11] = core::Real(0);
    mat[12] = core::Real(0);
    mat[13] = core::Real(0);
    mat[14] = core::Real(0);
    mat[15] = e;
}

gearoenix::math::Mat4x4::Mat4x4()
{
    mat[0] = core::Real(1);
    mat[1] = core::Real(0);
    mat[2] = core::Real(0);
    mat[3] = core::Real(0);
    mat[4] = core::Real(0);
    mat[5] = core::Real(1);
    mat[6] = core::Real(0);
    mat[7] = core::Real(0);
    mat[8] = core::Real(0);
    mat[9] = core::Real(0);
    mat[10] = core::Real(1);
    mat[11] = core::Real(0);
    mat[12] = core::Real(0);
    mat[13] = core::Real(0);
    mat[14] = core::Real(0);
    mat[15] = core::Real(1);
}

gearoenix::math::Mat4x4::Mat4x4(
    core::Real e0, core::Real e1, core::Real e2, core::Real e3,
    core::Real e4, core::Real e5, core::Real e6, core::Real e7,
    core::Real e8, core::Real e9, core::Real e10, core::Real e11,
    core::Real e12, core::Real e13, core::Real e14, core::Real e15)
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

gearoenix::math::Mat4x4::Mat4x4(system::File* f)
{
    read(f);
}

gearoenix::math::Mat4x4::Mat4x4(const Mat4x4& m)
{
    for (int i = 0; i < 16; i++) {
        mat[i] = m.mat[i];
    }
}

gearoenix::math::Vec3 gearoenix::math::Mat4x4::operator*(const Vec3& v) const
{
    return Vec3(mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + mat[12],
        mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + mat[13],
        mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + mat[14]);
}

gearoenix::math::Vec4 gearoenix::math::Mat4x4::operator*(const Vec4& v) const
{
    return Vec4(
        mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + mat[12] * v[3],
        mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + mat[13] * v[3],
        mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + mat[14] * v[3],
        mat[3] * v[0] + mat[7] * v[1] + mat[11] * v[2] + mat[15] * v[3]);
}

gearoenix::math::Mat4x4 gearoenix::math::Mat4x4::operator*(const Mat4x4& m) const
{
    Mat4x4 r;
    for (int i = 0, ri = 0; i < 16; i += 4) {
        for (int j = 0; j < 4; j++, ri++) {
            r.mat[ri] = m.mat[i] * mat[j] + m.mat[i + 1] * mat[j + 4] + m.mat[i + 2] * mat[j + 8] + m.mat[i + 3] * mat[j + 12];
        }
    }
    return r;
}

void gearoenix::math::Mat4x4::operator=(const Mat4x4& m)
{
    for (int i = 0; i < 16; i++) {
        mat[i] = m.mat[i];
    }
}

void gearoenix::math::Mat4x4::operator*=(const Mat4x4& m)
{
    Mat4x4 r;
    for (int i = 0, ri = 0; i < 16; i += 4) {
        for (int j = 0; j < 4; j++, ri++) {
            r.mat[ri] = m.mat[i] * mat[j] + m.mat[i + 1] * mat[j + 4] + m.mat[i + 2] * mat[j + 8] + m.mat[i + 3] * mat[j + 12];
        }
    }
    for (int i = 0; i < 16; ++i) {
        mat[i] = r.mat[i];
    }
}

void gearoenix::math::Mat4x4::scale3x3(const core::Real& s)
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

void gearoenix::math::Mat4x4::scale3x3(const core::Real& a, const core::Real& b, const core::Real& c)
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

void gearoenix::math::Mat4x4::scale4x3(const core::Real& s)
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

gearoenix::core::Real gearoenix::math::Mat4x4::determinant() const
{
    return (+mat[0] * (+mat[5] * (mat[10] * mat[15] - mat[11] * mat[14]) - mat[9] * (mat[6] * mat[15] - mat[7] * mat[14]) + mat[13] * (mat[6] * mat[11] - mat[7] * mat[10]))
        - mat[4] * (+mat[1] * (mat[10] * mat[15] - mat[11] * mat[14]) - mat[9] * (mat[2] * mat[15] - mat[3] * mat[14]) + mat[13] * (mat[2] * mat[11] - mat[3] * mat[10]))
        + mat[8] * (+mat[1] * (mat[6] * mat[15] - mat[7] * mat[14]) - mat[5] * (mat[2] * mat[15] - mat[3] * mat[14]) + mat[13] * (mat[2] * mat[7] - mat[3] * mat[6]))
        - mat[12] * (+mat[1] * (mat[6] * mat[11] - mat[7] * mat[10]) - mat[5] * (mat[2] * mat[11] - mat[3] * mat[10]) + mat[9] * (mat[2] * mat[7] - mat[3] * mat[6])));
}

void gearoenix::math::Mat4x4::set_location_zero()
{
    mat[12] = core::Real(0);
    mat[13] = core::Real(0);
    mat[14] = core::Real(0);
}

void gearoenix::math::Mat4x4::set_location(const Vec3& location)
{
    mat[12] = location[0];
    mat[13] = location[1];
    mat[14] = location[2];
}

void gearoenix::math::Mat4x4::read(system::File* f)
{
    for (int i = 0; i < 16; ++i)
        f->read(mat[i]);
}

gearoenix::math::Mat4x4 gearoenix::math::Mat4x4::inversed() const
{
    core::Real id = 1.0f / determinant();
    Mat4x4 result;
    result.mat[0] = id * (+mat[5] * (mat[10] * mat[15] - mat[11] * mat[14]) - mat[9] * (mat[6] * mat[15] - mat[7] * mat[14]) + mat[13] * (mat[6] * mat[11] - mat[7] * mat[10]));
    result.mat[1] = id * -(+mat[1] * (mat[10] * mat[15] - mat[11] * mat[14]) - mat[9] * (mat[2] * mat[15] - mat[3] * mat[14]) + mat[13] * (mat[2] * mat[11] - mat[3] * mat[10]));
    result.mat[2] = id * (+mat[1] * (mat[6] * mat[15] - mat[7] * mat[14]) - mat[5] * (mat[2] * mat[15] - mat[3] * mat[14]) + mat[13] * (mat[2] * mat[7] - mat[3] * mat[6]));
    result.mat[3] = id * -(+mat[1] * (mat[6] * mat[11] - mat[7] * mat[10]) - mat[5] * (mat[2] * mat[11] - mat[3] * mat[10]) + mat[9] * (mat[2] * mat[7] - mat[3] * mat[6]));
    result.mat[4] = id * -(+mat[4] * (mat[10] * mat[15] - mat[11] * mat[14]) - mat[8] * (mat[6] * mat[15] - mat[7] * mat[14]) + mat[12] * (mat[6] * mat[11] - mat[7] * mat[10]));
    result.mat[5] = id * (+mat[0] * (mat[10] * mat[15] - mat[11] * mat[14]) - mat[8] * (mat[2] * mat[15] - mat[3] * mat[14]) + mat[12] * (mat[2] * mat[11] - mat[3] * mat[10]));
    result.mat[6] = id * -(+mat[0] * (mat[6] * mat[15] - mat[7] * mat[14]) - mat[4] * (mat[2] * mat[15] - mat[3] * mat[14]) + mat[12] * (mat[2] * mat[7] - mat[3] * mat[6]));
    result.mat[7] = id * (+mat[0] * (mat[6] * mat[11] - mat[7] * mat[10]) - mat[4] * (mat[2] * mat[11] - mat[3] * mat[10]) + mat[8] * (mat[2] * mat[7] - mat[3] * mat[6]));
    result.mat[8] = id * (+mat[4] * (mat[9] * mat[15] - mat[11] * mat[13]) - mat[8] * (mat[5] * mat[15] - mat[7] * mat[13]) + mat[12] * (mat[5] * mat[11] - mat[7] * mat[9]));
    result.mat[9] = id * -(+mat[0] * (mat[9] * mat[15] - mat[11] * mat[13]) - mat[8] * (mat[1] * mat[15] - mat[3] * mat[13]) + mat[12] * (mat[1] * mat[11] - mat[3] * mat[9]));
    result.mat[10] = id * (+mat[0] * (mat[5] * mat[15] - mat[7] * mat[13]) - mat[4] * (mat[1] * mat[15] - mat[3] * mat[13]) + mat[12] * (mat[1] * mat[7] - mat[3] * mat[5]));
    result.mat[11] = id * -(+mat[0] * (mat[5] * mat[11] - mat[7] * mat[9]) - mat[4] * (mat[1] * mat[11] - mat[3] * mat[9]) + mat[8] * (mat[1] * mat[7] - mat[3] * mat[5]));
    result.mat[12] = id * -(+mat[4] * (mat[9] * mat[14] - mat[10] * mat[13]) - mat[8] * (mat[5] * mat[14] - mat[6] * mat[13]) + mat[12] * (mat[5] * mat[10] - mat[6] * mat[9]));
    result.mat[13] = id * (+mat[0] * (mat[9] * mat[14] - mat[10] * mat[13]) - mat[8] * (mat[1] * mat[14] - mat[2] * mat[13]) + mat[12] * (mat[1] * mat[10] - mat[2] * mat[9]));
    result.mat[14] = id * -(+mat[0] * (mat[5] * mat[14] - mat[6] * mat[13]) - mat[4] * (mat[1] * mat[14] - mat[2] * mat[13]) + mat[12] * (mat[1] * mat[6] - mat[2] * mat[5]));
    result.mat[15] = id * (+mat[0] * (mat[5] * mat[10] - mat[6] * mat[9]) - mat[4] * (mat[1] * mat[10] - mat[2] * mat[9]) + mat[8] * (mat[1] * mat[6] - mat[2] * mat[5]));
    return result;
}

gearoenix::math::Mat4x4 gearoenix::math::Mat4x4::look_at(const Vec3& position, const Vec3& target, const Vec3& up)
{
    Vec3 z = (target - position).normalized();
    Vec3 x = up.cross(z).normalized();
    Vec3 y = z.cross(x);
    Mat4x4 m;
    m.mat[0] = -x[0];
    m.mat[1] = y[0];
    m.mat[2] = -z[0];
    m.mat[3] = core::Real(0.0);
    m.mat[4] = -x[1];
    m.mat[5] = y[1];
    m.mat[6] = -z[1];
    m.mat[7] = core::Real(0.0);
    m.mat[8] = -x[2];
    m.mat[9] = y[2];
    m.mat[10] = -z[2];
    m.mat[11] = core::Real(0.0);
    m.mat[12] = x.dot(position);
    m.mat[13] = -(y.dot(position));
    m.mat[14] = z.dot(position);
    m.mat[15] = core::Real(1.0);
    return m;
}

gearoenix::math::Mat4x4 gearoenix::math::Mat4x4::rotation(const Vec3& v, core::Real degree)
{
    core::Real sinus = core::Real(sin(degree));
    core::Real cosinus = core::Real(cos(degree));
    core::Real oneminuscos = core::Real(1.0 - cosinus);
    Vec3 w = v.normalized();
    core::Real wx2 = w[0] * w[0];
    core::Real wxy = w[0] * w[1];
    core::Real wxz = w[0] * w[2];
    core::Real wy2 = w[1] * w[1];
    core::Real wyz = w[1] * w[2];
    core::Real wz2 = w[2] * w[2];
    core::Real wxyonemincos = wxy * oneminuscos;
    core::Real wxzonemincos = wxz * oneminuscos;
    core::Real wyzonemincos = wyz * oneminuscos;
    core::Real wxsin = w[0] * sinus;
    core::Real wysin = w[1] * sinus;
    core::Real wzsin = w[2] * sinus;
    Mat4x4 m;
    m.mat[0] = cosinus + (wx2 * oneminuscos);
    m.mat[1] = wzsin + wxyonemincos;
    m.mat[2] = wxzonemincos - wysin;
    m.mat[3] = core::Real(0.0);
    m.mat[4] = wxyonemincos - wzsin;
    m.mat[5] = cosinus + (wy2 * oneminuscos);
    m.mat[6] = wxsin + wyzonemincos;
    m.mat[7] = core::Real(0.0);
    m.mat[8] = wysin + wxzonemincos;
    m.mat[9] = wyzonemincos - wxsin;
    m.mat[10] = cosinus + (wz2 * oneminuscos);
    m.mat[11] = core::Real(0.0);
    m.mat[12] = core::Real(0.0);
    m.mat[13] = core::Real(0.0);
    m.mat[14] = core::Real(0.0);
    m.mat[15] = core::Real(1.0);
    return m;
}

gearoenix::math::Mat4x4 gearoenix::math::Mat4x4::translator(const Vec3& v)
{
    Mat4x4 r;
    r.mat[0] = core::Real(1);
    r.mat[1] = core::Real(0);
    r.mat[2] = core::Real(0);
    r.mat[3] = core::Real(0);
    r.mat[4] = core::Real(0);
    r.mat[5] = core::Real(1);
    r.mat[6] = core::Real(0);
    r.mat[7] = core::Real(0);
    r.mat[8] = core::Real(0);
    r.mat[9] = core::Real(0);
    r.mat[10] = core::Real(1);
    r.mat[11] = core::Real(0);
    r.mat[12] = v[0];
    r.mat[13] = v[1];
    r.mat[14] = v[2];
    r.mat[15] = core::Real(1);
    return r;
}

gearoenix::math::Mat4x4 gearoenix::math::Mat4x4::orthographic(core::Real width, core::Real height, core::Real near, core::Real far)
{
    Mat4x4 r;
    r.mat[0] = core::Real(2.0 / width);
    r.mat[1] = core::Real(0.0);
    r.mat[2] = core::Real(0.0);
    r.mat[3] = core::Real(0.0);
    r.mat[4] = core::Real(0.0);
    r.mat[5] = core::Real(2.0 / height);
    r.mat[6] = core::Real(0.0);
    r.mat[7] = core::Real(0.0);
    r.mat[8] = core::Real(0.0);
    r.mat[9] = core::Real(0.0);
    r.mat[10] = core::Real(2.0 / (near - far));
    r.mat[11] = core::Real(0.0);
    r.mat[12] = core::Real(0.0);
    r.mat[13] = core::Real(0.0);
    r.mat[14] = core::Real((far + near) / (near - far));
    r.mat[15] = core::Real(1.0);
    return r;
}

gearoenix::math::Mat4x4 gearoenix::math::Mat4x4::perspective(core::Real width, core::Real height, core::Real near, core::Real far)
{
    Mat4x4 r;
    r.mat[0] = core::Real((2.0 * near) / width);
    r.mat[1] = core::Real(0.0);
    r.mat[2] = core::Real(0.0);
    r.mat[3] = core::Real(0.0);
    r.mat[4] = core::Real(0.0);
    r.mat[5] = core::Real((2.0 * near) / height);
    r.mat[6] = core::Real(0.0);
    r.mat[7] = core::Real(0.0);
    r.mat[8] = core::Real(0.0);
    r.mat[9] = core::Real(0.0);
    r.mat[10] = core::Real((far + near) / (near - far));
    r.mat[11] = core::Real(-1.0);
    r.mat[12] = core::Real(0.0);
    r.mat[13] = core::Real(0.0);
    r.mat[14] = core::Real((2.0 * far * near) / (near - far));
    r.mat[15] = core::Real(0.0);
    return r;
}

gearoenix::math::Mat4x4 gearoenix::math::Mat4x4::transposed() const
{
    Mat4x4 r;
    for (int i = 0; i < 16; ++i) {
        r.mat[((i & 3) << 2) + (i >> 2)] = mat[i];
    }
    return r;
}

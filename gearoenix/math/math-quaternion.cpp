#include "math-quaternion.hpp"

gearoenix::math::Quat::Quat(){}

gearoenix::math::Quat::Quat(const core::Real x, const core::Real y, const core::Real z, const core::Real w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{}

gearoenix::math::Mat4x4 gearoenix::math::Quat::to_mat() const
{
    const core::Real xx = x * x;
    const core::Real xy = x * y;
    const core::Real xz = x * z;
    const core::Real xw = x * w;
    const core::Real yy = y * y;
    const core::Real yz = y * z;
    const core::Real yw = y * w;
    const core::Real zz = z * z;
    const core::Real zw = z * w;
    Mat4x4 m;
    m[0] = 1.0f - 2.0f * ( yy + zz );
    m[1] = 2.0f * ( xy + zw );
    m[2] = 2.0f * ( xz - yw );
    m[4] = 2.0f * ( xy - zw );
    m[5] = 1.0f - 2.0f * ( xx + zz );
    m[6] = 2.0f * ( yz + xw );
    m[8] = 2.0f * ( xz + yw );
    m[9] = 2.0f * ( yz - xw );
    m[10] = 1.0f - 2.0f * ( xx + yy );
    return m;
}

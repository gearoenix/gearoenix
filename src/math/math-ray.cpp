#include "math-ray.hpp"

gearoenix::math::Ray3::Ray3(const Vec3& origin, const Vec3& normalized_direction)
    : o(origin)
    , d(normalized_direction)
{
}

const gearoenix::math::Vec3& gearoenix::math::Ray3::get_origin() const
{
    return o;
}

const gearoenix::math::Vec3& gearoenix::math::Ray3::get_direction() const
{
    return d;
}

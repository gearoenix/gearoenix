#include "phs-untransformable.hpp"
#include "../math/math-matrix.hpp"

void gearoenix::physics::Untransferable::initialize_axis(const math::Mat4x4& t) const
{
    math::Vec3 l;
    t.get_location(l);
    const_cast<math::Vec3&>(x_axis) = (t * x_axis) - l;
    const_cast<math::Vec3&>(y_axis) = (t * y_axis) - l;
    const_cast<math::Vec3&>(z_axis) = (t * z_axis) - l;
    const_cast<math::Vec3&>(x_axis).normalize();
    const_cast<math::Vec3&>(y_axis).normalize();
    const_cast<math::Vec3&>(z_axis).normalize();
}

gearoenix::physics::Untransferable::Untransferable()
    : x_axis(1.0f, 0.0f, 0.0f)
    , y_axis(0.0f, 1.0f, 0.0f)
    , z_axis(0.0f, 0.0f, 1.0f)
{
}

gearoenix::physics::Untransferable::~Untransferable()
{
}

const gearoenix::math::Vec3& gearoenix::physics::Untransferable::get_x_axis() const
{
    return x_axis;
}

const gearoenix::math::Vec3& gearoenix::physics::Untransferable::get_y_axis() const
{
    return y_axis;
}

const gearoenix::math::Vec3& gearoenix::physics::Untransferable::get_z_axis() const
{
    return z_axis;
}

gearoenix::math::Vec3 gearoenix::physics::Untransferable::get_location() const
{
    math::Vec3 l;
    get_location(l);
    return l;
}

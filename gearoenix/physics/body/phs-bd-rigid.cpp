#include "phs-bd-rigid.hpp"

gearoenix::physics::body::Rigid::Rigid()
    : Body(Behaviour::RIGID)
{
}

gearoenix::physics::body::Rigid::~Rigid()
{
}

void gearoenix::physics::body::Rigid::apply_force_on_origin(const math::Vec3& force)
{
    acceleration += force / mass;
}

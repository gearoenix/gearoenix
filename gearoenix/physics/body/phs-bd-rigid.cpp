#include "phs-bd-rigid.hpp"

gearoenix::physics::body::Rigid::Rigid(const core::Id my_id, const std::shared_ptr<Transferable>& body_obj)
    : Body(my_id, Behaviour::RIGID, body_obj)
{
}

gearoenix::physics::body::Rigid::~Rigid()
{
}

void gearoenix::physics::body::Rigid::apply_force_on_origin(const math::Vec3& force)
{
    acceleration += force / mass;
}

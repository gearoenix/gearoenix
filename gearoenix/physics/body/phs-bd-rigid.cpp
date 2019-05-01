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
    this->force += force;
}

void gearoenix::physics::body::Rigid::apply_time(const core::Real delta_time)
{
    const core::Real hdt = delta_time * 0.5f;
    const math::Vec3 acc = (force + (speed * air_friction)) * inversed_mass;
    const math::Vec3 curspeed = acc * delta_time;
    const math::Vec3 translation = (curspeed * hdt) + (speed * delta_time);
    force = math::Vec3();
    speed += curspeed;
    //body_obj->translate(translation);
    /*const math::Vec3 rotacc = (rotational_force + (rotational_speed * rotational_air_friction)) * rotational_latency;
	const math::Vec3 rotation = (rotacc * hsdt) + (rotational_speed * delta_time);*/
    // todo
}

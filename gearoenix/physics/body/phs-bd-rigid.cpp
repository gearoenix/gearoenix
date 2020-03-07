#include "phs-bd-rigid.hpp"

gearoenix::physics::body::Rigid::Rigid(const core::Id id, const std::shared_ptr<Transferable>& body_obj)
    : Body(id, Behaviour::RIGID, body_obj)
{
}

gearoenix::physics::body::Rigid::~Rigid() = default;

void gearoenix::physics::body::Rigid::apply_force_on_origin(const math::Vec3<double>& f)
{
    force += f;
}

void gearoenix::physics::body::Rigid::apply_time(const double delta_time)
{
    const auto hdt = delta_time * 0.5;
    const auto acc = (force + (speed * air_friction)) * inverted_mass;
    const auto curspeed = acc * delta_time;
    const auto translation = (curspeed * hdt) + (speed * delta_time);
    force = math::Vec3<double>();
    speed += curspeed;
    //body_obj->translate(translation);
    /*const math::Vec3 rotacc = (rotational_force + (rotational_speed * rotational_air_friction)) * rotational_latency;
	const math::Vec3 rotation = (rotacc * hsdt) + (rotational_speed * delta_time);*/
    // todo
}

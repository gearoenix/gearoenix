#include "gx-phs-bd-rigid.hpp"

gearoenix::physics::body::Rigid::Rigid(
    const core::Id id,
    std::string name,
    std::shared_ptr<render::model::Dynamic> model) noexcept
    : Body(id, std::move(name), Type::Rigid, std::move(model))
{
}

gearoenix::physics::body::Rigid::~Rigid() noexcept = default;

void gearoenix::physics::body::Rigid::apply_force_on_origin(const math::Vec3<double>& f) noexcept
{
    force += f;
}

void gearoenix::physics::body::Rigid::apply_time(const double delta_time) noexcept
{
    const auto hdt = delta_time * 0.5;
    const auto acc = (force + (speed * air_friction)) * inverted_mass;
    const auto cur_speed = acc * delta_time;
    const auto translation = (cur_speed * hdt) + (speed * delta_time);
    force = math::Vec3<double>();
    speed += cur_speed;
    (void)translation;
    GX_UNIMPLEMENTED
    // body_obj->translate(translation);
    // const math::Vec3 rot_acc = (rotational_force + (rotational_speed * rotational_air_friction)) * rotational_latency;
    // const math::Vec3 rotation = (rot_acc * hdt) + (rotational_speed * delta_time);
    // todo
}

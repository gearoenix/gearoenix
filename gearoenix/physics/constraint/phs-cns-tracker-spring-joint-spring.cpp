#include "phs-cns-tracker-spring-joint-spring.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../render/engine/rnd-eng-engine.hpp"
#include "../../render/model/rnd-mdl-model.hpp"
#include "../../system/sys-app.hpp"
#include "../body/phs-bd-rigid.hpp"
#include <utility>

gearoenix::physics::constraint::TrackerSpringJointSpring::TrackerSpringJointSpring(
    const core::Id my_id,
    system::stream::Stream* const,
    render::engine::Engine* const,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&)
    : Constraint(my_id, Type::TRACKER_SPRING_JOINT_SPRING) {
        GXUNIMPLEMENTED
    }

    gearoenix::physics::constraint::TrackerSpringJointSpring::TrackerSpringJointSpring(const core::Id my_id, std::shared_ptr<body::Rigid> active, std::shared_ptr<body::Rigid> passive, const double k, const math::Vec3<double>& angle, const double joint_k, const double length)
    : Constraint(my_id, Type::TRACKER_SPRING_JOINT_SPRING)
    , active(std::move(active))
    , passive(std::move(passive))
    , k(k)
    , angle(angle)
    , joint_k(joint_k)
    , length(length)
{
}

gearoenix::physics::constraint::TrackerSpringJointSpring::~TrackerSpringJointSpring() = default;

void gearoenix::physics::constraint::TrackerSpringJointSpring::on_event(const core::event::Event&)
{
}

std::vector<std::pair<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Dynamic>>> gearoenix::physics::constraint::TrackerSpringJointSpring::get_all_models() const
{
    // maybe in future its need a change
    return std::vector<std::pair<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Dynamic>>>();
}

std::vector<std::shared_ptr<gearoenix::physics::body::Body>> gearoenix::physics::constraint::TrackerSpringJointSpring::get_all_bodies() const
{
    std::vector<std::shared_ptr<body::Body>> v(2);
    v[0] = active;
    v[1] = passive;
    return v;
}

void gearoenix::physics::constraint::TrackerSpringJointSpring::apply(const double) noexcept
{
    math::Vec3<double> actpos, paspos;
    //active->get_body_obj()->get_location(actpos);
    //passive->get_body_obj()->get_location(paspos);
    math::Vec3 direction = actpos - paspos;
    const double distance = direction.length();
    direction /= distance;
    const double delta_length = distance - length;
    if (delta_length > GXPOSEPSILON || delta_length < GXNEGEPSILON) {
        const double tracker_force = delta_length * k;
        passive->apply_force_on_origin(direction * tracker_force);
    }
    //const math::Vec3 cur_angle = ((active->get_body_obj()->get_x_axis() * angle[0]) + (active->get_body_obj()->get_y_axis() * angle[1]) + (active->get_body_obj()->get_z_axis() * angle[2])).normalized();
    //const double disang = 1.0f - direction.dot(cur_angle);
    //if (disang > GXPOSEPSILON) {
    //    const double angular_force = disang * joint_k;
    //    passive->apply_force_on_origin(direction.cross(cur_angle).cross(direction) * angular_force);
    //}
}

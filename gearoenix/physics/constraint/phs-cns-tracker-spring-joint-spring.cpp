#include "phs-cns-tracker-spring-joint-spring.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../render/engine/rnd-eng-engine.hpp"
#include "../../render/model/rnd-mdl-dynamic.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../body/phs-bd-rigid.hpp"

gearoenix::physics::constraint::TrackerSpringJointSpring::TrackerSpringJointSpring(
    const core::Id my_id,
    const std::shared_ptr<system::stream::Stream>& f,
    const std::shared_ptr<render::engine::Engine>& e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : Constraint(my_id, Type::TRACKER_SPRING_JOINT_SPRING)
{
    GXUNIMPLEMENTED;
}

gearoenix::physics::constraint::TrackerSpringJointSpring::TrackerSpringJointSpring(
    const core::Id my_id,
    const std::shared_ptr<body::Rigid>& active,
    const std::shared_ptr<body::Rigid>& passive,
    const core::Real k,
    const math::Vec3& angle,
    const core::Real joint_k,
    const core::Real length)
    : Constraint(my_id, Type::TRACKER_SPRING_JOINT_SPRING)
    , active(active)
    , passive(passive)
    , k(k)
    , angle(angle)
    , joint_k(joint_k)
    , length(length)
{
}

gearoenix::physics::constraint::TrackerSpringJointSpring::~TrackerSpringJointSpring()
{
}

void gearoenix::physics::constraint::TrackerSpringJointSpring::on_event(const core::event::Event&)
{
}

const std::vector<std::pair<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Dynamic>>> gearoenix::physics::constraint::TrackerSpringJointSpring::get_all_models() const
{
    // maybe in future its need a change
    return std::vector<std::pair<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Dynamic>>>();
}

const std::vector<std::shared_ptr<gearoenix::physics::body::Body>> gearoenix::physics::constraint::TrackerSpringJointSpring::get_all_bodies() const
{
    std::vector<std::shared_ptr<body::Body>> v(2);
    v[0] = active;
    v[1] = passive;
    return v;
}

void gearoenix::physics::constraint::TrackerSpringJointSpring::apply(core::Real)
{
    math::Vec3 actpos, paspos;
    active->get_body_obj()->get_location(actpos);
    passive->get_body_obj()->get_location(paspos);
    math::Vec3 direction = actpos - paspos;
    const core::Real distance = direction.length();
    direction /= distance;
    const core::Real delta_length = distance - length;
    if (delta_length > GXPOSEPSILON || delta_length < GXNEGEPSILON) {
        const core::Real tracker_force = delta_length * k;
        passive->apply_force_on_origin(direction * tracker_force);
    }
    //const math::Vec3 cur_angle = ((active->get_body_obj()->get_x_axis() * angle[0]) + (active->get_body_obj()->get_y_axis() * angle[1]) + (active->get_body_obj()->get_z_axis() * angle[2])).normalized();
    //const core::Real disang = 1.0f - direction.dot(cur_angle);
    //if (disang > GXPOSEPSILON) {
    //    const core::Real angular_force = disang * joint_k;
    //    passive->apply_force_on_origin(direction.cross(cur_angle).cross(direction) * angular_force);
    //}
}

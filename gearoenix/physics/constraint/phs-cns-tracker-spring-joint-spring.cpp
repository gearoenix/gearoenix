#include "phs-cns-tracker-spring-joint-spring.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../render/model/rnd-mdl-model.hpp"
#include "../../render/rnd-engine.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../body/phs-bd-rigid.hpp"

gearoenix::physics::constraint::TrackerSpringJointSpring::TrackerSpringJointSpring(system::stream::Stream*, render::Engine*, core::EndCaller<core::EndCallerIgnore>)
    : Constraint(TRACKER_SPRING_JOINT_SPRING)
{
    UNIMPLEMENTED;
}

gearoenix::physics::constraint::TrackerSpringJointSpring::TrackerSpringJointSpring(const std::shared_ptr<body::Rigid> active, const std::shared_ptr<body::Rigid> passive, const core::Real k, const math::Vec3 & angle, const core::Real joint_k, const core::Real length)
	: Constraint(TRACKER_SPRING_JOINT_SPRING)
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

void gearoenix::physics::constraint::TrackerSpringJointSpring::apply(core::Real)
{
    math::Vec3 actpos, paspos;
    active->get_location(actpos);
    passive->get_location(paspos);
    math::Vec3 direction = actpos - paspos;
    const core::Real distance = direction.length();
    direction /= distance;
    const core::Real delta_length = length - distance;
    if (delta_length > GXPOSEPSILON) {
        const core::Real tracker_force = delta_length * k;
        passive->apply_force_on_origin(direction * tracker_force);
    }
    const math::Vec3 cur_angle = ((active->get_x_axis() * angle[0]) + (active->get_y_axis() * angle[1]) + (active->get_z_axis() * angle[2])).normalized();
    const core::Real disang = 1.0f - direction.dot(cur_angle);
    if (disang > GXPOSEPSILON) {
        const core::Real angular_force = disang * joint_k;
        passive->apply_force_on_origin(direction.cross(cur_angle).cross(direction) * angular_force);
    }
}

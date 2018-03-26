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

gearoenix::physics::constraint::TrackerSpringJointSpring::TrackerSpringJointSpring(system::stream::Stream* f, render::Engine* render_engine, core::EndCaller<core::EndCallerIgnore> c)
    : Constraint(PLACER)
{
    //    f->read(t);
    //    switch (t) {
    //    case DOWN_MIDDLE:
    //        parameters.resize(5);
    //        // 0 -> x-middle
    //        // 1 -> y-down
    //        // 2 -> ratio
    //        // 3 -> size
    //        // 4 -> next_size
    //        f->read(parameters[2]);
    //        f->read(parameters[0]);
    //        f->read(parameters[1]);
    //        parameters[3] = 2.0f;
    //        parameters[4] = 2.0f * (render_engine->get_system_application()->get_window_ratio() - parameters[0]);
    //        next_position[1] = (parameters[4] / (parameters[2] * 2.0f)) + parameters[1] - 1.0f;
    //        break;
    //    case LEFT:
    //        parameters.resize(1);
    //        f->read(parameters[0]);
    //        position[0] = -1.0f;
    //        next_position[0] = parameters[0] - render_engine->get_system_application()->get_window_ratio();
    //        break;
    //    case RIGHT:
    //        parameters.resize(1);
    //        f->read(parameters[0]);
    //        position[0] = 1.0f;
    //        next_position[0] = render_engine->get_system_application()->get_window_ratio() - parameters[0];
    //        break;
    //    default:
    //        UNEXPECTED;
    //    }
    //    std::vector<core::Id> model_ids;
    //    f->read(model_ids);
    //    core::asset::Manager* asmgr = render_engine->get_system_application()->get_asset_manager();
    //    for (const core::Id model_id : model_ids) {
    //        models[model_id] = asmgr->get_model(model_id, core::EndCaller<render::model::Model>([c](std::shared_ptr<render::model::Model>) -> void {}));
    //    }
}

gearoenix::physics::constraint::TrackerSpringJointSpring::~TrackerSpringJointSpring()
{
}

void gearoenix::physics::constraint::TrackerSpringJointSpring::apply(core::Real delta_time)
{
    std::shared_ptr<body::Rigid> active, passive;
    if (!(active = this->active.lock()) || !(passive = this->passive.lock()) || active == nullptr || passive == nullptr) {
        alive = false;
        return;
    }
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

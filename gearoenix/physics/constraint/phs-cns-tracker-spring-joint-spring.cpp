#include "phs-cns-tracker-spring-joint-spring.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-window-resize.hpp"
#include "../../render/model/rnd-mdl-model.hpp"
#include "../../render/rnd-engine.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"

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

void gearoenix::physics::constraint::TrackerSpringJointSpring::apply()
{
    //    if (applied)
    //        return;
    //    Constraint::apply();
    //    switch (t) {
    //    case DOWN_MIDDLE: {
    //        for (const std::pair<core::Id, std::shared_ptr<render::model::Model>>& id_model : models)
    //            id_model.second->translate(-position);
    //        const core::Real scale = parameters[4] / parameters[3];
    //        for (const std::pair<core::Id, std::shared_ptr<render::model::Model>>& id_model : models)
    //            id_model.second->global_scale(scale);
    //        for (const std::pair<core::Id, std::shared_ptr<render::model::Model>>& id_model : models)
    //            id_model.second->translate(next_position);
    //        parameters[3] = parameters[4];
    //        break;
    //    }
    //    case RIGHT:
    //    case LEFT: {
    //        const math::Vec3 trn = next_position - position;
    //        for (const std::pair<core::Id, std::shared_ptr<render::model::Model>>& id_model : models)
    //            id_model.second->translate(trn);
    //        break;
    //    }
    //    default:
    //        UNEXPECTED;
    //    }
    //    position = next_position;
}

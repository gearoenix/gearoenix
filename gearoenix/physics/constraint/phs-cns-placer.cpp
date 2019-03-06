//#include "phs-cns-placer.hpp"
//#include "../../core/asset/cr-asset-manager.hpp"
//#include "../../core/event/cr-ev-event.hpp"
//#include "../../core/event/cr-ev-window-resize.hpp"
//#include "../../render/model/rnd-mdl-dynamic.hpp"
//#include "../../render/model/rnd-mdl-manager.hpp"
//#include "../../render/engine/rnd-eng-engine.hpp"
//#include "../../system/stream/sys-stm-stream.hpp"
//#include "../../system/sys-app.hpp"
//#include "../../system/sys-log.hpp"
//
//gearoenix::physics::constraint::Placer::Placer(
//	const core::Id my_id,
//	const std::shared_ptr<system::stream::Stream> &f,
//	const std::shared_ptr<render::engine::Engine> &e,
//	const core::sync::EndCaller<core::sync::EndCallerIgnore> c)
//    : Constraint(my_id, Type::PLACER)
//{
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
//        GXUNEXPECTED;
//    }
//    std::vector<core::Id> model_ids;
//    f->read(model_ids);
//    core::asset::Manager* asmgr = render_engine->get_system_application()->get_asset_manager();
//    for (const core::Id model_id : model_ids) {
//        std::shared_ptr<render::model::Model> mdl = asmgr->get_model_manager()->get(model_id, core::sync::EndCaller<render::model::Model>(c));
//        if (render::model::Model::RenderModel::STATIC == mdl->get_render_model_type())
//            GXUNEXPECTED;
//        models[model_id] = std::static_pointer_cast<render::model::Dynamic>(mdl);
//    }
//}
//
//gearoenix::physics::constraint::Placer::~Placer()
//{
//}
//
//void gearoenix::physics::constraint::Placer::apply(core::Real delta_time)
//{
//    if (applied)
//        return;
//    Constraint::apply(delta_time);
//    switch (t) {
//    case DOWN_MIDDLE: {
//        for (const std::pair<core::Id, std::shared_ptr<render::model::Dynamic>>& id_model : models)
//            id_model.second->translate(-position);
//        const core::Real scale = parameters[4] / parameters[3];
//        for (const std::pair<core::Id, std::shared_ptr<render::model::Dynamic>>& id_model : models)
//            id_model.second->global_scale(scale);
//        for (const std::pair<core::Id, std::shared_ptr<render::model::Dynamic>>& id_model : models)
//            id_model.second->translate(next_position);
//        parameters[3] = parameters[4];
//        break;
//    }
//    case RIGHT:
//    case LEFT: {
//        const math::Vec3 trn = next_position - position;
//        for (const std::pair<core::Id, std::shared_ptr<render::model::Dynamic>>& id_model : models)
//            id_model.second->translate(trn);
//        break;
//    }
//    default:
//        GXUNEXPECTED;
//    }
//    position = next_position;
//}
//
//void gearoenix::physics::constraint::Placer::on_event(const core::event::Event& e)
//{
//    if (e.get_type() != core::event::Event::From::WINDOW_RESIZE)
//        return;
//    const core::event::WindowResize& we = e.to_window_resize();
//    applied = false;
//    const core::Real screen_ratio = we.get_current_width() / we.get_current_height();
//    switch (t) {
//    case DOWN_MIDDLE: {
//        parameters[4] = 2.0f * (screen_ratio - parameters[0]);
//        next_position[1] = (parameters[4] / (parameters[2] * 2.0f)) + parameters[1] - 1.0f;
//        break;
//    }
//    case LEFT:
//        next_position[0] = parameters[0] - screen_ratio;
//        break;
//    case RIGHT:
//        next_position[0] = screen_ratio - parameters[0];
//        break;
//    default:
//        GXUNEXPECTED;
//    }
//}
//
//const std::vector<std::pair<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Dynamic>>> gearoenix::physics::constraint::Placer::get_all_models() const
//{
//    const size_t num_mdl = models.size();
//    std::vector<std::pair<core::Id, std::shared_ptr<render::model::Dynamic>>> result(num_mdl);
//    size_t i = 0;
//    for (auto iter = models.begin(); i < num_mdl; ++i, ++iter) {
//        result[i].first = iter->first;
//        result[i].second = iter->second;
//    }
//    return result;
//}

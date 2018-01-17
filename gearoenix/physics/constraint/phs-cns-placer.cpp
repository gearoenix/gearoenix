#include "phs-cns-placer.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-event.hpp"
#include "../../render/model/rnd-mdl-model.hpp"
#include "../../render/rnd-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"

gearoenix::physics::constraint::Placer::Placer(system::File* f, render::Engine* render_engine, core::EndCaller<core::EndCallerIgnore> c)
    : Constraint(PLACER)
{
    f->read(t);
    f->read(ratio);
    switch (t) {
    case DOWN_MIDDLE:
        parameters = new core::Real[2];
        f->read(parameters[0]);
        f->read(parameters[1]);
        next_size = 2.0f * (render_engine->get_system_application()->get_window_ratio() - parameters[0]);
        next_position[0] = 0.0f;
        next_position[1] = (next_size / (ratio * 2.0f)) + parameters[1] - 1.0f;
        next_position[2] = 0.0f;
        break;
    default:
        UNEXPECTED;
    }
    std::vector<core::Id> model_ids;
    f->read(model_ids);
    core::asset::Manager* asmgr = render_engine->get_system_application()->get_asset_manager();
    for (const core::Id model_id : model_ids) {
        models[model_id] = asmgr->get_model(model_id, core::EndCaller<render::model::Model>([c](std::shared_ptr<render::model::Model>) -> void {}));
    }
}

gearoenix::physics::constraint::Placer::~Placer()
{
    if (nullptr != parameters)
        delete[] parameters;
    parameters = nullptr;
}

void gearoenix::physics::constraint::Placer::apply()
{
    if (applied)
        return;
    Constraint::apply();
    switch (t) {
    case DOWN_MIDDLE: {
        for (const std::pair<core::Id, std::shared_ptr<render::model::Model>>& id_model : models)
            id_model.second->translate(-position);
        const core::Real scale = next_size / size;
        for (const std::pair<core::Id, std::shared_ptr<render::model::Model>>& id_model : models)
            id_model.second->global_scale(scale);
        for (const std::pair<core::Id, std::shared_ptr<render::model::Model>>& id_model : models)
            id_model.second->translate(next_position);
        position = next_position;
        size = next_size;
        break;
    }
    default:
        UNEXPECTED;
    }
}

void gearoenix::physics::constraint::Placer::on_event(const core::event::Event& e)
{
    applied = false;
    switch (t) {
    case DOWN_MIDDLE: {
        const core::event::WindowResize* we = e.to_window_resize();
        if (nullptr != we) {
            next_size = 2.0f * ((we->get_current_width() / we->get_current_height()) - parameters[0]);
            next_position[1] = (next_size / (ratio * 2.0f)) + parameters[1] - 1.0f;
        }
        break;
    }
    default:
        UNEXPECTED;
    }
}

const std::vector<std::pair<gearoenix::core::Id, std::shared_ptr<gearoenix::render::model::Model>>> gearoenix::physics::constraint::Placer::get_all_models() const
{
    const size_t num_mdl = models.size();
    std::vector<std::pair<core::Id, std::shared_ptr<render::model::Model>>> result(num_mdl);
    size_t i = 0;
    for (auto iter = models.begin(); i < num_mdl; ++i, ++iter) {
        result[i].first = iter->first;
        result[i].second = iter->second;
    }
    return result;
}

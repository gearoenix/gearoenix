#include "gx-phs-cns-window-scaler.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../../render/model/gx-rnd-mdl-model.hpp"
#include "../../render/model/gx-rnd-mdl-transformation.hpp"
#include "../../system/gx-sys-application.hpp"

void gearoenix::physics::constraint::WindowScaler::update() noexcept
{
    auto new_size = event_engine->get_window_ratio() * 2.0 * size_percentage;
    if (max_size_scale.has_value()) {
        if (new_size > *max_size_scale)
            new_size = *max_size_scale;
    }
    if (min_size_scale.has_value()) {
        if (new_size < *min_size_scale)
            new_size = *min_size_scale;
    }
    const auto scale = new_size / current_size;
    for (const auto& m : affected_models)
        m.second->get_transformation()->local_scale(scale);
    current_size = new_size;
    update_chained_constraints();
}

gearoenix::physics::constraint::WindowScaler::WindowScaler(const core::Id id, std::string name, render::engine::Engine* const e) noexcept
    : Constraint(id, Type::WindowScaler, std::move(name))
    , event_engine(e->get_system_application()->get_event_engine())
{
}

gearoenix::physics::constraint::WindowScaler::~WindowScaler() noexcept
{
    if (registered)
        event_engine->remove_listener(core::event::Id::SystemWindowSizeChange, 0.0, this);
}

bool gearoenix::physics::constraint::WindowScaler::on_event(const core::event::Data& e) noexcept
{
    switch (e.get_source()) {
    case core::event::Id::SystemWindowSizeChange: {
        WindowScaler::update();
        break;
    }
    default:
        GX_UNEXPECTED
    }
    return false;
}

void gearoenix::physics::constraint::WindowScaler::register_listener() noexcept
{
    event_engine->add_listener(core::event::Id::SystemWindowSizeChange, 0.0, this);
    registered = true;
}

void gearoenix::physics::constraint::WindowScaler::set_size_percentage(const double v) noexcept
{
    size_percentage = v;
    WindowScaler::update();
}

void gearoenix::physics::constraint::WindowScaler::set_max_size_scale(std::optional<double> v) noexcept
{
    max_size_scale = v;
    WindowScaler::update();
}

void gearoenix::physics::constraint::WindowScaler::set_min_size_scale(std::optional<double> v) noexcept
{
    min_size_scale = v;
    WindowScaler::update();
}

void gearoenix::physics::constraint::WindowScaler::set_current_size(const double v) noexcept
{
    current_size = v;
    WindowScaler::update();
}

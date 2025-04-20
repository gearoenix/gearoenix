#include "gx-rnd-wdg-layout.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"

void gearoenix::render::widget::LayoutWindowBasedPlacer::set_location(physics::Transformation& trn) const
{
    const auto wds = distance.to_world();
    const auto& wsz = platform::Application::get().get_base().get_window_size();
    const auto war = static_cast<double>(wsz.x) / static_cast<double>(wsz.y);
    auto l = trn.get_local_position();
    switch (side) {
    case Side::Top:
        l.y = std::max(1.0 / war, 1.0) * 0.5 - wds;
        break;
    case Side::Bottom:
        l.y = wds - std::max(1.0 / war, 1.0) * 0.5;
        break;
    case Side::Right:
        l.x = std::max(war, 1.0) * 0.5 - wds;
        break;
    case Side::Left:
        l.x = wds - std::max(war, 1.0) * 0.5;
        break;
    }
    trn.set_local_position(l);
}

gearoenix::core::event::Listener::Response gearoenix::render::widget::LayoutWindowBasedPlacer::on_event(const core::event::Data& event_data)
{
    if (event_data.get_source() == core::event::Id::PlatformWindowSizeChange) {
        if (auto* const trn = model_entity->get_component<physics::Transformation>(); nullptr != trn) {
            set_location(*trn);
        } else {
            GX_UNEXPECTED;
        }
    }
    return Response::Continue;
}

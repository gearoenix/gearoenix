// #include "gx-phs-cns-window-placer.hpp"
// #include "../../core/event/gx-cr-ev-engine.hpp"
// #include "../../platform/gx-plt-application.hpp"
// #include "../../render/engine/gx-rnd-eng-engine.hpp"
// #include "../../render/model/gx-rnd-mdl-model.hpp"
// #include "../../render/model/gx-rnd-mdl-transformation.hpp"
//
// void gearoenix::physics::constraint::WindowPlacer::update()
// {
//     const auto p = math::Vec3(distance() + math::Vec2((2.0 * width_percentage - 1.0) * event_engine->get_window_ratio(), 2.0 * height_percentage - 1.0), z);
//     for (const auto& m : affected_models)
//         m.second->get_transformation()->set_location(p);
//     update_chained_constraints();
// }
//
// gearoenix::physics::constraint::WindowPlacer::WindowPlacer(const core::Id id, std::string name, render::engine::Engine* const e)
//     : Constraint(id, Type::WindowPlacer, std::move(name))
//     , distance([] { return math::Vec2(0.0); })
//     , event_engine(e->get_platform_application()->get_event_engine())
// {
//     event_engine->add_listener(core::event::Id::SystemWindowSizeChange, 0.0, this);
// }
//
// gearoenix::physics::constraint::WindowPlacer::~WindowPlacer()
// {
//     event_engine->remove_listener(core::event::Id::SystemWindowSizeChange, 0.0, this);
// }
//
// bool gearoenix::physics::constraint::WindowPlacer::on_event(const core::event::Data& e)
// {
//     switch (e.get_source()) {
//     case core::event::Id::SystemWindowSizeChange: {
//         WindowPlacer::update();
//         break;
//     }
//     default:
//         GX_UNEXPECTED
//     }
//     return false;
// }
//
// void gearoenix::physics::constraint::WindowPlacer::set_width_percentage(const double v)
// {
//     width_percentage = v;
//     WindowPlacer::update();
// }
//
// void gearoenix::physics::constraint::WindowPlacer::set_height_percentage(const double v)
// {
//     height_percentage = v;
//     WindowPlacer::update();
// }
//
// void gearoenix::physics::constraint::WindowPlacer::set_z(const double v)
// {
//     z = v;
//     WindowPlacer::update();
// }
//
// void gearoenix::physics::constraint::WindowPlacer::set_distance(const DistanceFun& v)
// {
//     distance = v;
//     WindowPlacer::update();
// }
//
// void gearoenix::physics::constraint::WindowPlacer::set_distance(const gearoenix::math::Vec2<double>& v)
// {
//     set_distance([v] { return v; });
// }

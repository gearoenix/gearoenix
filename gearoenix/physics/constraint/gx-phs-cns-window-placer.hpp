// #pragma once
// #include "../../core/event/gx-cr-ev-listener.hpp"
// #include "../../math/gx-math-vector-3d.hpp"
// #include "gx-phs-cns-constraint.hpp"
// #include <functional>
// #include <map>
// #include <memory>
// #include <variant>
// #include <vector>
//
// namespace gearoenix::physics::constraint {
// struct WindowPlacer final : public Constraint, public core::event::Listener {
//     typedef std::function<math::Vec2<double>()> DistanceFun;
//     GX_GET_VAL_PRV(double, width_percentage, 0.0)
//     GX_GET_VAL_PRV(double, height_percentage, 0.0)
//     GX_GET_VAL_PRV(double, z, 0.0)
//     GX_GET_CREF_PRV(DistanceFun, distance)
//     GX_GET_CPTR_PRV(core::event::Engine, event_engine)
// private:
//     void update() override;
//
// public:
//     WindowPlacer(core::Id id, std::string name, render::engine::Engine* e);
//     ~WindowPlacer() override;
//
//     void apply(double, double) final { }
//
//     [[nodiscard]] bool on_event(const core::event::Data& e) override;
//
//     void set_width_percentage(double v);
//     void set_height_percentage(double v);
//     void set_z(double v);
//     void set_distance(const DistanceFun& v);
//     void set_distance(const math::Vec2<double>& v);
// };
// }
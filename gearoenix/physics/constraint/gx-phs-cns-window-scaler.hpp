// #pragma once
// #include "../../core/event/gx-cr-ev-listener.hpp"
// #include "../../math/gx-math-vector-3d.hpp"
// #include "gx-phs-cns-constraint.hpp"
// #include <map>
// #include <memory>
// #include <vector>
//
// namespace gearoenix::physics::constraint {
// struct WindowScaler final : public Constraint, public core::event::Listener {
//     GX_GET_VAL_PRV(double, size_percentage, 1.0)
//     GX_GET_VAL_PRV(std::optional<double>, max_size_scale, std::nullopt)
//     GX_GET_VAL_PRV(std::optional<double>, min_size_scale, std::nullopt)
//     GX_GET_VAL_PRV(double, current_size, 1.0)
//     GX_GET_VAL_PRV(bool, registered, false)
//     GX_GET_CPTR_PRV(core::event::Engine, event_engine)
//
// private:
//     void update() override;
//
// public:
//     WindowScaler(core::Id id, std::string name, render::engine::Engine* e);
//     ~WindowScaler() override;
//
//     void apply(double, double) final { }
//
//     [[nodiscard]] bool on_event(const core::event::Data& e) override;
//
//     void register_listener();
//
//     void set_size_percentage(double v);
//     void set_max_size_scale(std::optional<double> v);
//     void set_min_size_scale(std::optional<double> v);
//     void set_current_size(double s);
// };
// }
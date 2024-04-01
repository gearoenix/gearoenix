#ifndef GEAROENIX_PHYSICS_CONSTRAINT_WINDOW_SCALER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_WINDOW_SCALER_HPP

#include "../../core/event/gx-cr-ev-listener.hpp"
#include "../../core/gx-cr-types.hpp"
#include "../../core/sync/gx-cr-job-end-caller.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include "gx-phs-cns-constraint.hpp"
#include <map>
#include <memory>
#include <vector>

namespace gearoenix::physics::constraint {
struct WindowScaler final : public Constraint, public core::event::Listener {
    GX_GET_VAL_PRV(double, size_percentage, 1.0)
    GX_GET_VAL_PRV(std::optional<double>, max_size_scale, std::nullopt)
    GX_GET_VAL_PRV(std::optional<double>, min_size_scale, std::nullopt)
    GX_GET_VAL_PRV(double, current_size, 1.0)
    GX_GET_VAL_PRV(bool, registered, false)
    GX_GET_CPTR_PRV(core::event::Engine, event_engine)

private:
    void update() final;

public:
    WindowScaler(core::Id id, std::string name, render::engine::Engine* e);
    ~WindowScaler() final;

    void apply(double, double) final { }

    [[nodiscard]] bool on_event(const core::event::Data& e) final;

    void register_listener();

    void set_size_percentage(double v);
    void set_max_size_scale(std::optional<double> v);
    void set_min_size_scale(std::optional<double> v);
    void set_current_size(double s);
};
}
#endif

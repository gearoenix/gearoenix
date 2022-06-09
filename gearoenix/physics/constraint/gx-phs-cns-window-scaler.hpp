#ifndef GEAROENIX_PHYSICS_CONSTRAINT_WINDOW_SCALER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_WINDOW_SCALER_HPP

#include "../../core/event/gx-cr-ev-listener.hpp"
#include "../../core/gx-cr-types.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
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
    void update() noexcept final;

public:
    WindowScaler(core::Id id, std::string name, render::engine::Engine* e) noexcept;
    ~WindowScaler() noexcept final;

    void apply(double, double) noexcept final { }

    [[nodiscard]] bool on_event(const core::event::Data& e) noexcept final;

    void register_listener() noexcept;

    void set_size_percentage(double v) noexcept;
    void set_max_size_scale(std::optional<double> v) noexcept;
    void set_min_size_scale(std::optional<double> v) noexcept;
    void set_current_size(double s) noexcept;
};
}
#endif

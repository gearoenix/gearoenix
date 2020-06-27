#ifndef GEAROENIX_PHYSICS_CONSTRAINT_WINDOW_PLACER_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_WINDOW_PLACER_HPP

#include "../../core/cr-types.hpp"
#include "../../core/event/cr-ev-listener.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector-3d.hpp"
#include "phs-cns-constraint.hpp"
#include <functional>
#include <map>
#include <memory>
#include <variant>
#include <vector>

namespace gearoenix::physics::constraint {
class WindowPlacer final : public Constraint, public core::event::Listener {
    typedef std::function<math::Vec2<double>()> DistanceFun;
    GX_GET_VAL_PRV(double, width_percentage, 0.0)
    GX_GET_VAL_PRV(double, height_percentage, 0.0)
    GX_GET_VAL_PRV(double, z, 0.0)
    GX_GET_CREF_PRV(DistanceFun, distance)
    GX_GET_CPTR_PRV(core::event::Engine, event_engine)
private:
    void update() noexcept final;

public:
    WindowPlacer(core::Id id, std::string name, render::engine::Engine* e) noexcept;
    ~WindowPlacer() noexcept final;

    void apply(double, double) noexcept final { }

    [[nodiscard]] bool on_event(const core::event::Data& e) noexcept final;

    void set_width_percentage(double v) noexcept;
    void set_height_percentage(double v) noexcept;
    void set_z(double v) noexcept;
    void set_distance(const DistanceFun& v) noexcept;
    void set_distance(const math::Vec2<double>& v) noexcept;
};
}
#endif

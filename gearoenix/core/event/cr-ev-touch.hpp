#ifndef GEAROENIX_CORE_EVENT_TOUCH_HPP
#define GEAROENIX_CORE_EVENT_TOUCH_HPP
#include "cr-ev-point.hpp"
#include <chrono>

namespace gearoenix::core::event::touch {

typedef std::uint64_t FingerId;

struct State {
    GX_GET_CVAL_PRV(FingerId, finger_id)
    GX_GET_CREF_PRV(Point2D, point)

    State(const FingerId finger_id, const math::Vec2<int>& raw, const math::Vec2<double>& p) noexcept
        : finger_id(finger_id)
        , point(raw, p)
    {
    }

    State(const State& o) noexcept = default;
    State& operator=(const State& o) noexcept = default;

    void update(const math::Vec2<int>& raw, const math::Vec2<double>& p) noexcept
    {
        point.update(raw, p);
    }
};
}
#endif

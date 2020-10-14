#ifndef GEAROENIX_CORE_EVENT_TOUCH_HPP
#define GEAROENIX_CORE_EVENT_TOUCH_HPP
#include "gx-cr-ev-point.hpp"
#include <chrono>
#include <utility>

namespace gearoenix::core::event::touch {

typedef std::uint64_t FingerId;

struct State {
    GX_GET_CVAL_PRV(FingerId, finger_id)
    GX_GET_REFC_PRV(Point2D, point)

    State(const FingerId finger_id, const Point2D& point) noexcept
        : finger_id(finger_id)
        , point(point)
    {
    }
};

enum struct Action {
    Down,
    Up,
    Move,
    Cancel,
};

struct Data {
    GX_GET_REFC_PRV(State, state)
    GX_GET_CVAL_PRV(Action, action)

    Data(State state, const Action& action) noexcept
        : state(std::move(state))
        , action(action)
    {
    }
};

}
#endif

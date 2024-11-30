#pragma once
#include "../../platform/gx-plt-touch.hpp"
#include "gx-cr-ev-point.hpp"
#include <chrono>
#include <utility>

namespace gearoenix::core::event::touch {

enum struct Action {
    Down,
    Up,
    Move,
    Cancel,
};

struct Data final {
    GX_GET_REFC_PRV(Point2D, point);
    GX_GET_CVAL_PRV(platform::FingerId, finger_id);
    GX_GET_CVAL_PRV(Action, action);

public:
    Data(Point2D point, const platform::FingerId finger_id, const Action& action)
        : point(point)
        , finger_id(finger_id)
        , action(action)
    {
    }
};

}
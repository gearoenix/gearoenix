#ifndef GEAROENIX_CORE_EVENT_TOUCH_HPP
#define GEAROENIX_CORE_EVENT_TOUCH_HPP
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
    Data(Point2D point, const platform::FingerId finger_id, const Action& action) noexcept
        : point(point)
        , finger_id(finger_id)
        , action(action)
    {
    }
};

}
#endif

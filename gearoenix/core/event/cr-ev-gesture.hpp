#ifndef GEAROENIX_CORE_EVENT_GESTURE_HPP
#define GEAROENIX_CORE_EVENT_GESTURE_HPP
#include "cr-ev-button.hpp"
#include "cr-ev-point.hpp"
#include <chrono>

namespace gearoenix::core::event::gesture {
struct Drag2D {
    GX_GET_CREF_PRV(Point2D, point)

    Drag2D(const math::Vec2<int>& raw, const math::Vec2<double>& p) noexcept
        : point(raw, p)
    {
    }

    void update(const math::Vec2<int>& raw, const math::Vec2<double>& p) noexcept
    {
        point.update(raw, p);
    }
};

struct MouseDrag {
    GX_GET_CREF_PRV(Drag2D, base)
    button::MouseKeyId key = button::MouseKeyId::Unknown;
};
}
#endif

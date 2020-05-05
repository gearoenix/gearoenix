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

    explicit Drag2D(const Point2D& p) noexcept : point(p) {}

    Drag2D(const Drag2D&) noexcept = default;
    Drag2D&operator=(const Drag2D&) noexcept = default;

    void update(const math::Vec2<int>& raw, const math::Vec2<double>& p) noexcept
    {
        point.update(raw, p);
    }
};

struct MouseDrag {
    GX_GET_CREF_PRV(Drag2D, base)
    GX_GET_CVAL_PRV(button::MouseKeyId, key)

    MouseDrag(const Drag2D& d, const button::MouseKeyId k) noexcept : base(d), key(k) {}

    MouseDrag(const MouseDrag& c) noexcept = default;
    MouseDrag& operator=(const MouseDrag& c) noexcept = default;

};
}
#endif

#ifndef GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#define GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#include "cr-ev-point.hpp"
#include <chrono>

namespace gearoenix::core::event::movement {
struct Base2D {
    GX_GET_CREF_PRV(Point2D, point)
    GX_GET_CREF_PRV(math::Vec2<double>, raw_speed)
    GX_GET_CREF_PRV(math::Vec2<double>, speed)

    Base2D(const math::Vec2<int>& raw, const math::Vec2<double>& p) noexcept
        : point(raw, p)
    {
    }

    Base2D() noexcept : point({0, 0}, {0.0, 0.0}) {}

    void update(const math::Vec2<int>& raw, const math::Vec2<double>& p) noexcept
    {
        point.update(raw, p);
        raw_speed = math::Vec2<double>(point.get_delta_raw_previous_position()) / point.get_delta_previous_time();
        speed = point.get_delta_previous_position() / point.get_delta_previous_time();
    }
};
}
#endif

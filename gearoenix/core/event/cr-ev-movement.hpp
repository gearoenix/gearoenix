#ifndef GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#define GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#include "cr-ev-point.hpp"
#include <chrono>

namespace gearoenix::core::event::movement {
struct Base2D {
    GX_GET_CREF_PRV(Point2D, point)

    math::Vec2<double> raw_speed;
    math::Vec2<double> speed;

    Base2D(const math::Vec2<double>& raw, const math::Vec2<double>& p) noexcept
        : point(raw, p)
    {
    }

    void update(const math::Vec3<int>& raw, const math::Vec3<double>& p) noexcept
    {

        speed = delta_position / delta_time;
    }
};
}
#endif

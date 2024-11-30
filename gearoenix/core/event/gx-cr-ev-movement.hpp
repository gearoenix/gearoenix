#pragma once
#include "gx-cr-ev-point.hpp"
#include <chrono>

namespace gearoenix::core::event::movement {
struct Base2D final {
    GX_GET_REFC_PRV(Point2D, point);
    GX_GET_REFC_PRV(math::Vec2<double>, raw_speed);
    GX_GET_REFC_PRV(math::Vec2<double>, speed);

public:
    explicit Base2D(const Point2D& point)
        : point(point)
        , raw_speed(math::Vec2<double>(point.get_delta_raw_previous_position()) / point.get_delta_previous_time())
        , speed(point.get_delta_previous_position() / point.get_delta_previous_time())
    {
    }
};

struct Mouse final {
    GX_GET_REFC_PRV(math::Vec2<double>, current_position);
    GX_GET_REFC_PRV(math::Vec2<double>, current_normalised_position);
    GX_GET_REFC_PRV(math::Vec2<double>, previous_position);
    GX_GET_REFC_PRV(math::Vec2<double>, previous_normalised_position);
    GX_GET_REFC_PRV(math::Vec2<double>, delta_position);
    GX_GET_REFC_PRV(math::Vec2<double>, delta_normalised_position);

public:
    Mouse(
        const math::Vec2<double>& current_position,
        const math::Vec2<double>& current_normalised_position,
        const math::Vec2<double>& previous_position,
        const math::Vec2<double>& previous_normalised_position)
        : current_position(current_position)
        , current_normalised_position(current_normalised_position)
        , previous_position(previous_position)
        , previous_normalised_position(previous_normalised_position)
        , delta_position(current_position - previous_position)
        , delta_normalised_position(current_normalised_position - previous_normalised_position)
    {
    }
};
}
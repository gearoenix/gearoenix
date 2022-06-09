#ifndef GEAROENIX_CORE_EVENT_POINT_HPP
#define GEAROENIX_CORE_EVENT_POINT_HPP
#include "../../math/gx-math-vector-3d.hpp"
#include <chrono>

namespace gearoenix::core::event {

struct Point2D final {
    GX_GET_CREF_PRV(std::chrono::high_resolution_clock::time_point, start_time)
    GX_GET_CREF_PRV(std::chrono::high_resolution_clock::time_point, previous_time)
    GX_GET_CREF_PRV(std::chrono::high_resolution_clock::time_point, current_time)
    GX_GET_CREF_PRV(math::Vec2<double>, start_position)
    GX_GET_CREF_PRV(math::Vec2<double>, previous_position)
    GX_GET_CREF_PRV(math::Vec2<double>, current_position)
    GX_GET_CREF_PRV(math::Vec2<double>, delta_start_position)
    GX_GET_CREF_PRV(math::Vec2<double>, delta_previous_position)
    GX_GET_CREF_PRV(math::Vec2<double>, raw_start_position)
    GX_GET_CREF_PRV(math::Vec2<double>, raw_previous_position)
    GX_GET_CREF_PRV(math::Vec2<double>, raw_current_position)
    GX_GET_CREF_PRV(math::Vec2<double>, delta_raw_start_position)
    GX_GET_CREF_PRV(math::Vec2<double>, delta_raw_previous_position)
    GX_GET_VAL_PRV(double, delta_start_time, 0.0)
    GX_GET_VAL_PRV(double, delta_previous_time, 0.0)

    Point2D(const math::Vec2<double>& raw, const math::Vec2<double>& p) noexcept
        : start_time(std::chrono::high_resolution_clock::now())
        , previous_time(start_time)
        , current_time(start_time)
        , start_position(p)
        , previous_position(p)
        , current_position(p)
        , raw_start_position(raw)
        , raw_previous_position(raw)
        , raw_current_position(raw)
    {
    }

    Point2D(const Point2D& o) noexcept = default;
    Point2D& operator=(const Point2D& o) noexcept = default;

    void update(const math::Vec2<double>& raw, const math::Vec2<double>& p) noexcept
    {
        previous_position = current_position;
        raw_previous_position = raw_current_position;

        current_position = p;
        raw_current_position = raw;

        delta_previous_position = current_position - previous_position;
        delta_start_position = start_position - previous_position;

        delta_raw_previous_position = raw_current_position - raw_previous_position;
        delta_raw_start_position = raw_start_position - raw_previous_position;

        previous_time = current_time;
        current_time = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> dur = current_time - previous_time;
        delta_previous_time = dur.count();
        dur = current_time - start_time;
        delta_start_time = dur.count();
    }

    void reinitialize() noexcept
    {
        start_time = previous_time = current_time = std::chrono::high_resolution_clock::now();
        start_position = previous_position = current_position;
        raw_start_position = raw_previous_position = raw_current_position;
        delta_start_position = delta_previous_position = math::Vec2(0.0);
        delta_raw_start_position = delta_raw_previous_position = math::Vec2(0.0);
        delta_start_time = delta_previous_time = 0.0;
    }
};
}
#endif

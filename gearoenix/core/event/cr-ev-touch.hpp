#ifndef GEAROENIX_CORE_EVENT_TOUCH_HPP
#define GEAROENIX_CORE_EVENT_TOUCH_HPP
#include "../../math/math-vector-2d.hpp"
#include "../cr-static.hpp"
#include <chrono>

namespace gearoenix::core::event::touch {

typedef std::uint64_t FingerId;

struct State {
    GX_GET_CVAL_PRV(FingerId, finger_id)
    GX_GET_CREF_PRV(std::chrono::high_resolution_clock::time_point, start_time)
    GX_GET_CREF_PRV(std::chrono::high_resolution_clock::time_point, previous_time)
    GX_GET_CREF_PRV(std::chrono::high_resolution_clock::time_point, current_time)
    GX_GET_CREF_PRV(math::Vec2<double>, start_position)
    GX_GET_CREF_PRV(math::Vec2<double>, previous_position)
    GX_GET_CREF_PRV(math::Vec2<double>, current_position)
    GX_GET_CREF_PRV(math::Vec2<int>, raw_start_position)
    GX_GET_CREF_PRV(math::Vec2<int>, raw_previous_position)
    GX_GET_CREF_PRV(math::Vec2<int>, raw_current_position)
    GX_GET_CREF_PRV(math::Vec2<double>, delta_start_position)
    GX_GET_CREF_PRV(math::Vec2<double>, delta_previous_position)
    GX_GET_CREF_PRV(math::Vec2<int>, delta_raw_start_position)
    GX_GET_CREF_PRV(math::Vec2<int>, delta_raw_previous_position)
    GX_GET_VAL_PRV(double, delta_start_time, 0.0)
    GX_GET_VAL_PRV(double, delta_previous_time, 0.0)

    State(const FingerId finger_id, const math::Vec2<int>& raw, const math::Vec2<double>& p) noexcept
        : finger_id(finger_id)
        , start_time(std::chrono::high_resolution_clock::now())
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

    State(const State& o) noexcept = default;
    State& operator=(const State& o) noexcept = default;

    void update(const math::Vec2<int>& raw, const math::Vec2<double>& p) noexcept
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

        std::chrono::duration<double> dur = previous_time - current_time;
        delta_previous_time = dur.count();
        dur = start_time - current_time;
        delta_start_time = dur.count();
    }
};
}
#endif

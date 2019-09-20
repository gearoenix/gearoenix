#ifndef GEAROENIX_CORE_EVENT_GESTURE_HPP
#define GEAROENIX_CORE_EVENT_GESTURE_HPP
#include "../../math/math-vector.hpp"
#include "../cr-types.hpp"
#include "cr-ev-button.hpp"
#include <chrono>
namespace gearoenix::core::event::gesture {
struct Drag {
    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point current_time = std::chrono::high_resolution_clock::now();

    math::Vec3 start_position;
    math::Vec3 current_position;
    math::Vec3 previous_position;

    math::Vec3 delta_previous_position;
    math::Vec3 delta_start_position;

    Real delta_previous_time;
    Real delta_start_time;

    void update(const math::Vec3& cp) noexcept
    {
        current_position = cp;

        delta_previous_position = current_position - previous_position;
        delta_start_position = start_position - previous_position;

        current_time = std::chrono::high_resolution_clock::now();

        std::chrono::duration<Real> dur = previous_time - current_time;
        delta_previous_time = dur.count();
        dur = start_time - current_time;
        delta_start_time = dur.count();
    }
};

struct MouseDrag {
    Drag base;
    button::MouseKeyId key;
};
}
#endif

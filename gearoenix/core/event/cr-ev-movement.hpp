#ifndef GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#define GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#include "../../math/math-vector-3d.hpp"
#include "../cr-types.hpp"
#include <chrono>

namespace gearoenix::core::event::movement {
struct Base {
    std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point current_time = std::chrono::high_resolution_clock::now();

    math::Vec3<double> current_position;
    math::Vec3<double> previous_position;

    math::Vec3<double> delta_position;
    double delta_time = 0.0;

    math::Vec3<double> speed;

    void update(const math::Vec3<double>& p) noexcept
    {
        previous_position = current_position;
        previous_time = current_time;
        current_position = p;
        delta_position = current_position - previous_position;
        current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dt = current_time - previous_time;
        delta_time = dt.count();
        speed = delta_position / delta_time;
    }
};
}
#endif

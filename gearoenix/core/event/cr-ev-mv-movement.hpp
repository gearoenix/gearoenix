#ifndef GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#define GEAROENIX_CORE_EVENT_MOVEMENT_HPP
#include "../cr-types.hpp"
#include <chrono>
namespace gearoenix::core::event::movement {
struct Data {
    std::chrono::high_resolution_clock::time_point previous_position = std::chrono::high_resolution_clock::now();
    core::Real x = 0.0f;
    core::Real y = 0.0f;
    core::Real z = 0.0f;
    core::Real pre_x = 0.0f;
    core::Real pre_y = 0.0f;
    core::Real pre_z = 0.0f;
    core::Real dx = 0.0f;
    core::Real dy = 0.0f;
    core::Real dz = 0.0f;
};
}
#endif

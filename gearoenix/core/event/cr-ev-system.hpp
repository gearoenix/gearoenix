#ifndef GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP
#define GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP
#include "../cr-types.hpp"
namespace gearoenix::core::event::system {
struct WindowSizeChangeData {
    std::size_t previous_width = 0;
    std::size_t previous_height = 0;
    double previous_reversed_half_width = 0.0;
    double previous_reversed_half_height = 0.0;
    double previous_ratio = 0.0;
    std::size_t current_width = 0;
    std::size_t current_height = 0;
    double current_reversed_half_width = 0.0;
    double current_reversed_half_height = 0.0;
    double current_ratio = 0.0;
    std::size_t delta_width = 0;
    std::size_t delta_height = 0;
    double delta_reversed_half_width = 0.0;
    double delta_reversed_half_height = 0.0;
    double delta_ratio = 0.0;
};
}
#endif
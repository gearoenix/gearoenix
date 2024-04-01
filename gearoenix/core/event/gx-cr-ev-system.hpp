#ifndef GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP
#define GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP

namespace gearoenix::core::event::platform {
struct WindowSizeChangeData final {
    const int previous_width;
    const int previous_height;
    const double previous_reversed_half_width;
    const double previous_reversed_half_height;
    const double previous_ratio;
    const int current_width;
    const int current_height;
    const double current_reversed_half_width;
    const double current_reversed_half_height;
    const double current_ratio;
    const int delta_width;
    const int delta_height;
    const double delta_reversed_half_width;
    const double delta_reversed_half_height;
    const double delta_ratio;

    WindowSizeChangeData(
        const int previous_width,
        const int previous_height,
        const int current_width,
        const int current_height)
        : previous_width(previous_width)
        , previous_height(previous_height)
        , previous_reversed_half_width(2.0 / static_cast<double>(previous_width))
        , previous_reversed_half_height(2.0 / static_cast<double>(previous_height))
        , previous_ratio(previous_reversed_half_height / previous_reversed_half_width)
        , current_width(current_width)
        , current_height(current_height)
        , current_reversed_half_width(2.0 / static_cast<double>(current_width))
        , current_reversed_half_height(2.0 / static_cast<double>(current_height))
        , current_ratio(current_reversed_half_height / current_reversed_half_width)
        , delta_width(current_width - previous_width)
        , delta_height(current_height - previous_height)
        , delta_reversed_half_width(current_reversed_half_width - previous_reversed_half_width)
        , delta_reversed_half_height(current_reversed_half_height - previous_reversed_half_height)
        , delta_ratio(current_ratio - previous_ratio)
    {
    }
};
}
#endif
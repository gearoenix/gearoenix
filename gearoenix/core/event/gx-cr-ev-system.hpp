#pragma once

namespace gearoenix::core::event::platform {
struct WindowSizeChangeData final {
    const int previous_width;
    const int previous_height;
    const fp_t previous_reversed_half_width;
    const fp_t previous_reversed_half_height;
    const fp_t previous_ratio;
    const int current_width;
    const int current_height;
    const fp_t current_reversed_half_width;
    const fp_t current_reversed_half_height;
    const fp_t current_ratio;
    const int delta_width;
    const int delta_height;
    const fp_t delta_reversed_half_width;
    const fp_t delta_reversed_half_height;
    const fp_t delta_ratio;

    WindowSizeChangeData(const int previous_width, const int previous_height, const int current_width, const int current_height)
        : previous_width(previous_width)
        , previous_height(previous_height)
        , previous_reversed_half_width(static_cast<fp_t>(2) / static_cast<fp_t>(previous_width))
        , previous_reversed_half_height(static_cast<fp_t>(2) / static_cast<fp_t>(previous_height))
        , previous_ratio(previous_reversed_half_height / previous_reversed_half_width)
        , current_width(current_width)
        , current_height(current_height)
        , current_reversed_half_width(static_cast<fp_t>(2) / static_cast<fp_t>(current_width))
        , current_reversed_half_height(static_cast<fp_t>(2) / static_cast<fp_t>(current_height))
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
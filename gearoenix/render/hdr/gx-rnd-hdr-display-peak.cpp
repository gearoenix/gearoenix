#include "gx-rnd-hdr-display-peak.hpp"

#include <SDL3/SDL_video.h>

#include <algorithm>
#include <cmath>

gearoenix::render::hdr::DisplayPeakInfo gearoenix::render::hdr::query_window_peak(SDL_Window* const window) noexcept
{
    DisplayPeakInfo info;
    if (nullptr == window) {
        return info;
    }
    const auto props = SDL_GetWindowProperties(window);
    if (0 == props) {
        return info;
    }
    info.hdr_enabled = SDL_GetBooleanProperty(props, SDL_PROP_WINDOW_HDR_ENABLED_BOOLEAN, false);
    info.headroom = SDL_GetFloatProperty(props, SDL_PROP_WINDOW_HDR_HEADROOM_FLOAT, 1.0f);
    if (info.headroom < 1.0f) {
        info.headroom = 1.0f;
    }
    return info;
}

std::uint8_t gearoenix::render::hdr::closest_agx_peak_index(const DisplayPeakInfo info, const float format_max) noexcept
{
    if (!info.hdr_enabled) {
        return 0; // SDR
    }
    const auto peak = info.headroom > 1.0f
        ? std::min(info.headroom, format_max) // Both sources agree on a real number; encoding caps the display.
        : format_max; // Incoherent: HDR_ENABLED but no headroom data; trust the swapchain's max.

    // Snap to the closest variant by absolute distance. Variants are monotonically increasing.
    std::uint8_t best_index = 0;
    auto best_distance = std::abs(peak - agx_peak_ratios[0]);
    for (std::uint8_t i = 1; i < agx_peak_ratios.size(); ++i) {
        const auto distance = std::abs(peak - agx_peak_ratios[i]);
        if (distance < best_distance) {
            best_distance = distance;
            best_index = i;
        }
    }
    return best_index;
}


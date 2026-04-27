#pragma once
#include <array>
#include <cstdint>

struct SDL_Window;

namespace gearoenix::render::hdr {
/// Snapshot of an SDL3 window's current HDR state: whether the OS reports HDR is active and the
/// headroom (multiplier above SDR white the panel can emit). When `hdr_enabled` is true but
/// `headroom == 1.0`, treat it as "incoherent": the OS says HDR is on but won't tell us how
/// bright -- the combiner uses the swapchain format's max as a fallback.
struct DisplayPeakInfo final {
    bool hdr_enabled = false;
    float headroom = 1.0f;
};

/// Discrete AgX peak ratios the swapchain snaps to when picking an HDR target (in SDR-white
/// multiples). 10 variants spanning SDR through HDR4000. The shader takes the float value directly
/// as a push constant -- the index is just for snapping logic + UI display.
inline constexpr std::array<float, 10> agx_peak_ratios = {
    1.0f, 1.5f, 2.0f, 3.0f, 4.0f, 6.0f, 8.0f, 10.0f, 15.0f, 20.0f
};

/// Query the window's display HDR state via SDL3 properties (`SDL_PROP_WINDOW_HDR_*`). Works
/// uniformly across Windows / Linux / macOS / iOS / Android since SDL3 abstracts each platform's
/// native query (CAMetalLayer EDR / Display.HdrCapabilities / etc.).
[[nodiscard]] DisplayPeakInfo query_window_peak(SDL_Window* window) noexcept;

/// Combine an SDL3-reported `DisplayPeakInfo` with the swapchain's `format_max` (in SDR-white
/// multiples) and snap to the closest AgX variant index. Per the agreed policy:
///   - `!hdr_enabled`                            -> peak = 1.0 -> index 0 (SDR)
///   - `hdr_enabled && headroom > 1.0`           -> peak = min(headroom, format_max) -> snap
///   - `hdr_enabled && headroom == 1.0` (incoherent) -> peak = format_max -> snap (capped at 9)
///
/// `format_max` must come from the actual swapchain format (storage capacity), not from the OETF
/// alone -- e.g., BGRA8_sRGB and RGBA16F_scRGB both use OETF=Linear but their HDR ceilings differ.
[[nodiscard]] std::uint8_t closest_agx_peak_index(DisplayPeakInfo info, float format_max) noexcept;
}

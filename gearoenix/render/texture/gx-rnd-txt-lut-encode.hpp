#pragma once
#include <cmath>

namespace gearoenix::render::texture {
/// Log encoding used to map HDR linear -> [0,1] before a 3D LUT lookup. The LUT input axis is
/// log2-space anchored at scene-referred 18% middle gray, with a symmetric ±7 EV window around it.
/// This matches Unreal Engine's tonemapping LUT convention (and the de-facto game-engine standard
/// shared by Unity HDRP / Frostbite / Decima): middle gray sits at the centre of the LUT axis so
/// graders' "stops above/below middle gray" intuition lines up with LUT samples.
///
/// Both the LUT pass shaders and the LUT generators must agree on these values.
///
/// Mapping (log_in in [0,1]):
///   log_in = 0.0  -> linear ~= 0.0014 (7 stops below 0.18 middle gray)
///   log_in = 0.5  -> linear  = 0.18   (middle gray, the anchor)
///   log_in = 1.0  -> linear ~= 23.04  (7 stops above 0.18 middle gray)
constexpr float lut_log_grey = 0.18f;
constexpr float lut_log_range = 14.0f; // total EV (= 2 * half-range)
constexpr float lut_log_eps = lut_log_grey * 0.0078125f; // = 0.18 * 2^-7, log_in=0 floor

/// Map LUT input axis [0,1] back to scene-referred linear. Used by LUT generators when computing
/// voxel values.
[[nodiscard]] inline float lut_log_to_linear(const float log_in) noexcept
{
    return std::exp2((log_in - 0.5f) * lut_log_range) * lut_log_grey;
}

/// Inverse: encode linear -> [0,1] log axis. Values <=0 map to 0; out-of-range values clamp at the
/// closest end.
[[nodiscard]] inline float lut_linear_to_log(const float linear) noexcept
{
    if (linear <= 0.0f) {
        return 0.0f;
    }
    const auto t = std::log2(linear / lut_log_grey) / lut_log_range + 0.5f;
    return t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
}
}

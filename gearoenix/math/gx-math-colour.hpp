#pragma once
#include <algorithm>
#include <array>
#include <cmath>

namespace gearoenix::math {
/// Linear -> sRGB transfer function (OETF), per IEC 61966-2-1, applied per-channel.
/// Input expected in [0, 1]. Returns sRGB-encoded value in [0, 1].
[[nodiscard]] inline float linear_to_srgb_channel(const float c) noexcept
{
    return c <= 0.0031308f ? c * 12.92f : 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
}

/// Inverse of `linear_to_srgb_channel`: sRGB -> linear (EOTF). Input in [0, 1].
[[nodiscard]] inline float srgb_to_linear_channel(const float c) noexcept
{
    return c <= 0.04045f ? c * (1.0f / 12.92f) : std::pow((c + 0.055f) * (1.0f / 1.055f), 2.4f);
}

/// PQ (SMPTE ST 2084) OETF, applied per-channel. Input is *normalized* linear where 1.0 corresponds
/// to PQ peak (10000 nits). The caller scales their LUT output by `sdr_white_nits / 10000` before
/// calling this. Returns PQ-encoded value in [0, 1].
[[nodiscard]] inline float linear_to_pq_channel(const float linear) noexcept
{
    constexpr float m1 = 0.1593017578125f;
    constexpr float m2 = 78.84375f;
    constexpr float c1 = 0.8359375f;
    constexpr float c2 = 18.8515625f;
    constexpr float c3 = 18.6875f;
    const auto yp = std::pow(std::max(linear, 0.0f), m1);
    return std::pow((c1 + c2 * yp) / (1.0f + c3 * yp), m2);
}

/// HLG (BT.2100) OETF, applied per-channel. Input is *normalized* scene-linear in [0, 1] where 1.0
/// is HLG signal peak (~1000 nits with system gamma 1.2). The engine's HLG path doesn't pass
/// `engine_linear * (sdr/peak)` directly -- it must apply the system-gamma inverse first (i.e.
/// scene_E = (engine_linear * sdr / peak)^(1/gamma)) so the resulting display luminance lands at
/// `engine_linear * sdr` nits per BT.2408 (signal 0.75 -> 200 nits at 1000-nit peak).
/// Returns HLG-encoded signal in [0, 1].
[[nodiscard]] inline float linear_to_hlg_channel(const float linear) noexcept
{
    constexpr float a = 0.17883277f;
    constexpr float b = 0.28466892f;
    constexpr float c = 0.55991073f;
    const auto e = std::max(linear, 0.0f);
    return e <= 1.0f / 12.0f ? std::sqrt(3.0f * e) : a * std::log(12.0f * e - b) + c;
}

/// AgX (Troy Sobotka) tonemap reference, used as the C++ source of truth that the colour-tuning
/// shaders (`gx-vk-shd-colour-tuning.comp`, `gx-gl-shd-colour-tuning.cpp`) inline per-pixel. The
/// shaders compute AgX directly now (no baked LUTs), so this header isn't called at runtime --
/// it documents the canonical constants the GLSL must match. Per-channel: input matrix slightly
/// desaturates, log-encode into the AgX EV window, 6th-degree contrast polynomial sigmoid, then
/// post-multiply by `peak_ratio` (1.0 = SDR white, 4.0 = 400-nit HDR, ...). Post-multiplying the
/// SDR sigmoid keeps the shoulder shape at SDR and linearly extends the bright region; the
/// proper HDR-aware AgX with a peak-conditioned sigmoid is future work.
struct AgxRgb final {
    float r, g, b;
};

[[nodiscard]] inline AgxRgb agx_input_matrix_apply(const AgxRgb v) noexcept
{
    return {
        0.842479062253094f * v.r + 0.0784335999999992f * v.g + 0.0792237451477643f * v.b,
        0.0423282422610123f * v.r + 0.878468636469772f * v.g + 0.0791661274605434f * v.b,
        0.0423756549057051f * v.r + 0.0784336f * v.g + 0.879142973793104f * v.b,
    };
}

/// Per-channel: clamp linear to AgX's EV window then normalise to [0,1] log axis.
[[nodiscard]] inline float agx_log_encode_channel(const float linear) noexcept
{
    constexpr float min_ev = -12.47393f;
    constexpr float max_ev = 4.026069f;
    constexpr float ev_range = max_ev - min_ev;
    const auto safe = std::max(linear, 1e-10f);
    return std::clamp((std::log2(safe) - min_ev) / ev_range, 0.0f, 1.0f);
}

/// AgX's "default contrast" sigmoid as a 6th-degree polynomial fit, per-channel. Input expected
/// in [0,1] (the log-encoded value). Output approximately in [0,1] for SDR-target.
[[nodiscard]] inline float agx_default_contrast_approx_channel(const float x) noexcept
{
    const auto x2 = x * x;
    const auto x4 = x2 * x2;
    return 15.5f * x4 * x2
        - 40.14f * x4 * x
        + 31.96f * x4
        - 6.868f * x2 * x
        + 0.4298f * x2
        + 0.1191f * x
        - 0.00232f;
}

[[nodiscard]] inline AgxRgb agx_tonemap(const AgxRgb linear, const float peak_ratio) noexcept
{
    const auto matched = agx_input_matrix_apply(linear);
    const AgxRgb log_encoded {
        agx_log_encode_channel(matched.r),
        agx_log_encode_channel(matched.g),
        agx_log_encode_channel(matched.b),
    };
    return {
        agx_default_contrast_approx_channel(log_encoded.r) * peak_ratio,
        agx_default_contrast_approx_channel(log_encoded.g) * peak_ratio,
        agx_default_contrast_approx_channel(log_encoded.b) * peak_ratio,
    };
}
}
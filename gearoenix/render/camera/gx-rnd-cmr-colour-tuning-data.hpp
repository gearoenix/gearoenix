#pragma once
#include "gx-rnd-cmr-tonemap-mode.hpp"

#include <memory>

namespace gearoenix::core {
struct Object;
}

namespace gearoenix::render::texture {
struct Texture3D;
}

namespace gearoenix::render::camera {
/// Per-camera colour-tuning state -- the inputs the colour-tuning compute pass needs from a
/// camera. Identity / AgX run inline in the shader (no LUT). CustomLut samples the user-imported
/// `colour_lut` 3D texture. AgX peak tracks the swapchain's currently picked HDR peak by default;
/// the UI can switch to a manual override.
struct ColourTuningData final {
    /// 3D LUT for the `CustomLut` tonemap mode. Null in Identity / AgX modes -- the bindless 3D
    /// descriptor at the camera's slot is allowed to be invalid in those modes (PartiallyBound on
    /// Vulkan, GL drivers tolerate the dynamically skipped sample similarly).
    std::shared_ptr<texture::Texture3D> colour_lut;
    /// Tonemap branch the colour-tuning shader takes. Default Identity preserves the renderer's
    /// scene-linear output without compression -- the right choice for HDRi-as-backdrop scenes
    /// where the artistic intent is "preserve the punchy clipped highlights". Users can opt into
    /// AgX (Sobotka, HDR-aware) or ACES (Narkowicz, SDR-only) when they want filmic compression.
    TonemapMode tonemap_mode = TonemapMode::Identity;
    /// AgX HDR peak ratio (in SDR-white multiples) used when the user has manually overridden via
    /// UI. Ignored while `track_swapchain_peak` is true.
    float agx_peak_ratio = 1.0f;
    /// When true (default), dispatch reads the swapchain's currently picked peak each frame so
    /// HDR-state changes track automatically without observer / inspector machinery. When false,
    /// dispatch uses `agx_peak_ratio` (manual override from the UI slider).
    bool track_swapchain_peak = true;

    /// Resolve the effective AgX peak the dispatch should push to the shader: the swapchain's
    /// live-picked peak when tracking, the manual override otherwise.
    [[nodiscard]] float effective_agx_peak() const;

    /// Resolve the tonemap the dispatch should actually push. CustomLut without an imported LUT
    /// silently falls back to Identity -- the shader's CustomLut branch divides by `lut_size`, so
    /// landing there with `lut_size = 0` would feed Inf/NaN UVs into the texture sampler.
    [[nodiscard]] TonemapMode effective_tonemap_mode() const;

    /// Render the colour-tuning inspector tree node. `object_self` is the camera's weak_ptr<Object>
    /// (passed in because we need it to keep the camera alive across the async `.cube` import).
    void show_debug_gui(const std::weak_ptr<core::Object>& object_self);
};
}

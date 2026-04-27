#pragma once
#include <cstdint>

namespace gearoenix::render::camera {
/// Per-camera colour-tuning mode. The colour-tuning shader branches on this and either does the
/// transform inline (Identity / AgX / Aces) or samples the 3D LUT bound via `colour_lut`
/// (CustomLut). Inline modes don't need a LUT bound at all -- the bindless 3D-texture descriptor
/// at the camera's `lut_texture_index` is allowed to be invalid (PartiallyBound on Vulkan).
///
/// Adding a new tonemap is four small steps -- this enum (UI order is matched by enum value, so
/// keep them aligned), the Vulkan shader branch in `gx-vk-shd-colour-tuning.comp`, the GL shader
/// branch in `gx-gl-shd-colour-tuning.cpp`, and the preset name in
/// `ColourTuningData::show_debug_gui`. Push-constant fields are shared across modes, so most
/// transforms that need extra parameters can repurpose `agx_peak_ratio` or piggyback a new field.
enum struct TonemapMode : std::uint32_t {
    /// Scene-linear pass-through. The cheapest path and the engine default; HDR clipping in SDR
    /// is the user's choice (e.g., HDRi sky backgrounds where the artistic intent is "preserve the
    /// punchy clipped highlights").
    Identity = 0,
    /// AgX (Sobotka) tonemap, computed per-pixel in the shader. `agx_peak_ratio` selects the HDR
    /// target (1.0 = SDR-white, 4.0 = HDR400-style, ..., 20.0 = HDR4000-style). Submission updates
    /// the active peak from the swapchain's reported display headroom each frame.
    AgX = 1,
    /// Narkowicz ACES filmic fit -- a fast 5-coefficient rational approximation widely used in
    /// game engines. SDR-only (output saturates near 1.0); for HDR, prefer AgX.
    Aces = 2,
    /// Sample a user-provided 3D LUT via `Camera::colour_lut`. Used by the `.cube` import path.
    CustomLut = 3,
};
}

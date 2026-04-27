#pragma once
#include <cstdint>
#include <vector>

namespace gearoenix::render::texture {
struct CubeLutData final {
    /// `size`^3 RGBA16F pixels, depth-major (Z-slice-by-Z-slice), each slice row-major.
    std::vector<std::uint8_t> pixels_rgba16f;
    std::uint32_t size = 0;
};

/// Whether the source `.cube`'s input axis is already log-encoded with the engine's
/// log range (±7 EV around 0.18 middle gray; see `gx-rnd-txt-lut-encode.hpp`). We expose only this
/// knob to artists (defaulted ON): tonemap choice is artistic and is baked into the LUT outputs
/// (not the input axis), and any gamma-encoded `.cube` is treated as the equivalent linear input
/// -- the engine is OETF-agnostic at import. Other log encodings (ACEScct, LogC, ...) must be
/// converted externally first.
enum struct CubeInputLogEncoding : std::uint8_t {
    None = 0,        ///< Linear input axis (the source samples linear values in [0, 1]).
    EngineRange = 1, ///< Already log2-encoded with the engine's middle-gray log range.
};

/// Parse an Adobe `.cube` 3D LUT file. Logs fatal on malformed input. Pixel layout matches what a
/// 3D texture sampler expects: R axis varies fastest, then G, then B - identical to `.cube`'s
/// on-disk order.
[[nodiscard]] CubeLutData parse_cube_lut(const char* text, std::size_t size);

/// Resample a `.cube` LUT into the engine's canonical log2-input encoding. `log_encoding` says
/// whether the source's input axis is already engine-log-encoded; if so, we short-circuit. If not,
/// the resampling composes the inverse: target_log_in -> linear -> sample.
[[nodiscard]] CubeLutData resample_cube_to_log(
    const CubeLutData& src,
    CubeInputLogEncoding log_encoding,
    std::uint32_t target_size = 128);
}

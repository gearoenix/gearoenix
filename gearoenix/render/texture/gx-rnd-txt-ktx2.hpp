#pragma once
#include "gx-rnd-txt-format.hpp"

#include <cstdint>
#include <vector>

namespace gearoenix::render::texture {
struct DecodedKtx2 final {
    std::vector<std::uint8_t> pixels;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t depth = 0;
    TextureFormat format = TextureFormat::Unknown;
};

/// Encode a 3D image as a KTX2 byte buffer. Always Zstd-supercompressed at the maximum level
/// the linked Zstd library exposes — the engine prefers smallness over encode speed.
/// `pixels` must hold `width * height * depth * format_pixel_size(format)` bytes,
/// laid out Z-slice by Z-slice (depth-major), with each slice stored row-major.
[[nodiscard]] std::vector<std::uint8_t> encode_ktx2_3d(
    const std::uint8_t* pixels,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t depth,
    TextureFormat format);

/// Decode a KTX2 byte buffer holding a single-level, single-layer 3D texture.
[[nodiscard]] DecodedKtx2 decode_ktx2_3d(const std::uint8_t* bytes, std::size_t size);
}

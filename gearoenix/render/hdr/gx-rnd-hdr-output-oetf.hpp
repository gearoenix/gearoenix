#pragma once
#include <cstdint>

namespace gearoenix::render::hdr {
/// Which OETF the colour-tuning shader applies to its already-tonemapped output before writing
/// to the swapchain. Picked at runtime per-frame as a `uint` push-constant / uniform; switch is
/// warp-coherent, so the cost is one rational-function eval per pixel.
enum struct OutputOetf : std::uint32_t {
    /// Linear pass-through. Used when the swapchain is a float HDR format (Apple EDR / scRGB
    /// linear) where the OS expects linear-light values, OR when the swapchain is `*_SRGB` and
    /// the GPU does the sRGB encoding for us on the swapchain writing.
    Linear = 0,
    /// Manual sRGB OETF in shader. Used for `*_UNORM` SDR swapchains.
    Srgb = 1,
    /// SMPTE ST 2084 (HDR10). The shader pre-scales the LUT output by `sdr_white_nits / 10000`
    /// before applying.
    Pq = 2,
    /// BT.2100 hybrid log-gamma. The shader pre-scales appropriately.
    Hlg = 3,
};

}

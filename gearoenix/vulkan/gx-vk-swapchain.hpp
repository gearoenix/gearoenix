#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-singleton.hpp"
#include "../render/hdr/gx-rnd-hdr-output-oetf.hpp"
#include "gx-vk-build-configuration.hpp"
#include "gx-vk-loader.hpp"

#include <array>
#include <cstdint>
#include <memory>

namespace gearoenix::vulkan::image {
struct View;
}

namespace gearoenix::vulkan::sync {
struct Semaphore;
}

namespace gearoenix::vulkan {
struct Swapchain final : core::Singleton<Swapchain> {
    struct Frame final {
        std::shared_ptr<image::View> view;
        std::unique_ptr<sync::Semaphore> present;
    };

    using frames_t = std::array<Frame, frames_in_flight>;

private:
    vk::SurfaceFormatKHR format { };
    vk::raii::SwapchainKHR vulkan_data { nullptr };
    frames_t frames;
    bool is_valid = true;
    std::uint32_t image_index = 0;
    /// OETF the colour-tuning shader applies before writing to the swapchain. Picked alongside the
    /// swapchain format (A7); A6 ships defaults (Linear / 80 nits) so the shader compiles.
    render::hdr::OutputOetf active_oetf = render::hdr::OutputOetf::Linear;
    /// PQ/HLG pre-scale nits for SDR white. SDL3-reported on macOS/iOS, 80 nits as a sane fallback.
    float active_sdr_white_nits = 80.0f;
    /// Multiplier the shader applies in the Linear OETF branch. scRGB float swapchains need
    /// `sdr_white_nits / 80` (so engine SDR-white lands at the platform's reported SDR nits, since
    /// scRGB convention is 1.0 = 80 nits absolute on Windows / 1.0 = user SDR-white relative on
    /// Apple). sRGB-encoded BGRA8 swapchains keep this at 1.0 (signal-space framebuffer).
    float active_linear_scale = 1.0f;
    /// One LSB in the chosen framebuffer's bit depth, used as the blue-noise dither scale. 1/255
    /// for 8-bit, 1/1023 for 10-bit, 0 for float (no banding to break). Calibrated dither breaks
    /// quantization banding without piling on excess noise on higher-precision formats.
    float active_dither_lsb = 1.0f / 255.0f;
    /// Index into `hdr::agx_peak_ratios` matching the active swapchain peak. Submission feeds this
    /// to texture::Manager so default-LUT fetches the right AgX variant.
    std::uint8_t active_agx_peak_index = 0;

public:
    [[nodiscard]] const vk::SurfaceFormatKHR& get_format() const { return format; }
    [[nodiscard]] vk::SwapchainKHR get_vulkan_data() const { return *vulkan_data; }
    [[nodiscard]] const frames_t& get_frames() const { return frames; }
    [[nodiscard]] bool get_is_valid() const { return is_valid; }
    [[nodiscard]] std::uint32_t get_image_index() const { return image_index; }
    [[nodiscard]] render::hdr::OutputOetf get_active_oetf() const { return active_oetf; }
    [[nodiscard]] float get_active_sdr_white_nits() const { return active_sdr_white_nits; }
    [[nodiscard]] float get_active_linear_scale() const { return active_linear_scale; }
    [[nodiscard]] float get_active_dither_lsb() const { return active_dither_lsb; }
    [[nodiscard]] std::uint8_t get_active_agx_peak_index() const { return active_agx_peak_index; }

    Swapchain();
    Swapchain(Swapchain&&) = delete;
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    ~Swapchain() override;

    /// If the frame is valid, it returns true otherwise false.
    void acquire_next_image(const sync::Semaphore& semaphore);
    void initialize();
    [[nodiscard]] const Frame& get_frame() const { return frames[image_index]; }
    void present();
    [[nodiscard]] const sync::Semaphore& get_present_semaphore() const;
};
}
#endif
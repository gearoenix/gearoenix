#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../render/camera/gx-rnd-cmr-manager.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>

namespace gearoenix::vulkan::pipeline {
struct Cache;
struct Pipeline;
}

namespace gearoenix::vulkan::shader {
struct Module;
}

namespace gearoenix::render::texture {
struct Texture2D;
struct Texture3D;
}

namespace gearoenix::vulkan::camera {

struct BloomPushConstants final {
    float texel_size_x;
    float texel_size_y;
    float src_mip;
    float value;
    float scatter_clamp_max_threshold_threshold_knee[4];
};

static_assert(sizeof(BloomPushConstants) == 32);

struct ColourTuningPushConstants final {
    std::uint32_t lut_texture_index; ///< Used only when `tonemap_mode == CustomLut`.
    std::uint32_t lut_sampler_index; ///< Used only when `tonemap_mode == CustomLut`.
    std::uint32_t noise_texture_index;
    std::uint32_t noise_sampler_index;
    std::uint32_t frame_index;
    std::uint32_t oetf_mode; ///< Matches `render::hdr::OutputOetf`.
    /// Matches `render::camera::TonemapMode` (Identity / AgX / Aces / CustomLut). Inline modes
    /// (Identity / AgX / Aces) bypass the bindless 3D-texture sample entirely, so no LUT needs to
    /// be bound for those modes (PartiallyBound covers the unused descriptor).
    std::uint32_t tonemap_mode;
    /// AgX peak ratio in SDR-white multiples (1.0 = SDR, 4.0 = 400-nit-style HDR, ...). Ignored
    /// when `tonemap_mode != AgX`. Pushed per frame so HDR-state changes apply on the next dispatch.
    float agx_peak_ratio;
    float lut_size; ///< Used only when `tonemap_mode == CustomLut`.
    float sdr_white_nits; ///< Used by PQ/HLG OETF pre-scale; ignored for Linear/Srgb.
    /// Multiplier the Linear OETF branch applies. `sdr_white_nits / 80` for scRGB float swapchains
    /// (so engine SDR-white lands at the platform's reported SDR nits); 1.0 for sRGB-encoded BGRA8
    /// swapchains (signal-space). Ignored for Srgb / Pq / Hlg modes.
    float linear_scale;
    /// One LSB at the swapchain's bit depth: 1/255 for 8-bit, 1/1023 for 10-bit, 0 for float.
    /// Calibrated so blue-noise dither breaks banding without over-dithering higher-precision
    /// formats. Float swapchains skip dither entirely (no quantization to break).
    float dither_lsb;
    float noise_uv_scale_x;
    float noise_uv_scale_y;
};

static_assert(sizeof(ColourTuningPushConstants) == 56);

struct Manager final : render::camera::Manager, core::Singleton<Manager> {
    [[nodiscard]] vk::DescriptorSetLayout get_bloom_descriptor_set_layout() const { return *bloom_descriptor_set_layout; }
    [[nodiscard]] vk::Sampler get_bloom_sampler() const { return *bloom_sampler; }
    [[nodiscard]] vk::PipelineLayout get_bloom_pipeline_layout() const { return *bloom_pipeline_layout; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_multiply_pipeline() const { return bloom_multiply_pipeline; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_prefilter_pipeline() const { return bloom_prefilter_pipeline; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_horizontal_pipeline() const { return bloom_horizontal_pipeline; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_vertical_pipeline() const { return bloom_vertical_pipeline; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_upsampler_pipeline() const { return bloom_upsampler_pipeline; }
    [[nodiscard]] vk::PipelineLayout get_colour_tuning_pipeline_layout() const { return *colour_tuning_pipeline_layout; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_colour_tuning_pipeline() const { return colour_tuning_pipeline; }
    GX_GET_CREF_PRT(std::shared_ptr<render::texture::Texture2D>, blue_noise);

private:
    vk::raii::DescriptorSetLayout bloom_descriptor_set_layout { nullptr };
    vk::raii::Sampler bloom_sampler { nullptr };
    vk::raii::PipelineLayout bloom_pipeline_layout { nullptr };
    std::shared_ptr<pipeline::Pipeline> bloom_multiply_pipeline;
    std::shared_ptr<pipeline::Pipeline> bloom_prefilter_pipeline;
    std::shared_ptr<pipeline::Pipeline> bloom_horizontal_pipeline;
    std::shared_ptr<pipeline::Pipeline> bloom_vertical_pipeline;
    std::shared_ptr<pipeline::Pipeline> bloom_upsampler_pipeline;

    vk::raii::PipelineLayout colour_tuning_pipeline_layout { nullptr };
    std::shared_ptr<pipeline::Pipeline> colour_tuning_pipeline;

    std::shared_ptr<pipeline::Cache> bloom_pipeline_cache;
    std::shared_ptr<shader::Module> bloom_multiply_shader_module;
    std::shared_ptr<shader::Module> bloom_prefilter_shader_module;
    std::shared_ptr<shader::Module> bloom_horizontal_shader_module;
    std::shared_ptr<shader::Module> bloom_vertical_shader_module;
    std::shared_ptr<shader::Module> bloom_upsampler_shader_module;
    std::shared_ptr<shader::Module> colour_tuning_shader_module;

    void initialise_bloom();
    void terminate_bloom();
    /// The colour-tuning compute pipeline samples the LUT/blue-noise via the bindless texture
    /// arrays, so its layout depends on `Bindless::get_descriptor_set_layout()`. The engine
    /// constructs `camera::Manager` in its constructor body *after* `descriptor::Bindless` is
    /// alive (not in the member init list), which is what makes this safe to call from here.
    void initialise_colour_tuning();
    void terminate_colour_tuning();

public:
    Manager();
    ~Manager() override;
    void build(std::string&& name, core::ecs::Entity* parent, core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;
    void window_resized() override;
    void update() override;
};
}

#endif

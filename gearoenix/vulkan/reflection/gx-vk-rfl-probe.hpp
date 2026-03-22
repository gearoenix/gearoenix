#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"

namespace gearoenix::vulkan::texture {
struct TextureCube;
}

namespace gearoenix::vulkan::reflection {
struct Probe {
    constexpr static auto max_count = render::reflection::Probe::max_count;
    constexpr static auto object_type_index = gearoenix_gapi_reflection_probe_type_index;
    constexpr static auto all_parent_object_type_indices = core::Object::all_parent_object_type_indices;
    constexpr static auto immediate_parent_object_type_indices = core::Object::immediate_parent_object_type_indices;

protected:
    std::uint32_t irradiance_texture_index = static_cast<std::uint32_t>(-1);
    std::uint32_t irradiance_sampler_index = static_cast<std::uint32_t>(-1);

    std::uint32_t radiance_texture_index = static_cast<std::uint32_t>(-1);
    std::uint32_t radiance_sampler_index = static_cast<std::uint32_t>(-1);

    float radiance_lod_coefficient = 0.0f;

    std::uint32_t shader_data_index = static_cast<std::uint32_t>(-1);

    std::uint64_t last_frame_number_update = static_cast<std::uint64_t>(-1);

    void set_textures(const texture::TextureCube& irr, const texture::TextureCube& rad);

public:
    [[nodiscard]] std::uint32_t get_irradiance_texture_index() const { return irradiance_texture_index; }
    [[nodiscard]] std::uint32_t get_irradiance_sampler_index() const { return irradiance_sampler_index; }
    [[nodiscard]] std::uint32_t get_radiance_texture_index() const { return radiance_texture_index; }
    [[nodiscard]] std::uint32_t get_radiance_sampler_index() const { return radiance_sampler_index; }
    [[nodiscard]] float get_radiance_lod_coefficient() const { return radiance_lod_coefficient; }
    [[nodiscard]] std::uint32_t get_shader_data_index() const { return shader_data_index; }

    virtual ~Probe();
    void after_record(std::uint64_t frame_number);
};
}
#endif
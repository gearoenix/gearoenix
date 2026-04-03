#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-rfl-probe.hpp"

#include <array>

namespace gearoenix::vulkan::texture {
struct Target;
struct TextureCube;
}

namespace gearoenix::vulkan::reflection {
struct Runtime final : render::reflection::Runtime, Probe {
    GEAROENIX_OBJECT_STRUCT_DEF;

    struct IrradiancePushConstants final {
        float u_axis[4];
        float v_axis[4];
        float face_axis[3];
        std::uint32_t image_size;
    };

    struct RadiancePushConstants final {
        float u_axis[4];
        float v_axis[4];
        float face_axis[3];
        std::uint32_t image_size;
        float roughness;
        float roughness_p_4;
        float sa_texel;
    };

    using GapiCubeTarget = std::array<std::shared_ptr<texture::Target>, 6>;
    using GapiMippedCubeTarget = std::array<boost::container::static_vector<std::shared_ptr<texture::Target>, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6>;
    using CubeDescriptorSet = std::array<vk::DescriptorSet, 6>;
    using MippedCubeDescriptorSet = std::array<boost::container::static_vector<vk::DescriptorSet, GX_RENDER_MAX_RUNTIME_REFLECTION_MIPMAPS_COUNT>, 6>;

    constexpr static auto max_count = render::reflection::Runtime::max_count;
    constexpr static auto object_type_index = gearoenix_gapi_reflection_runtime_type_index;
    constexpr static std::array all_parent_object_type_indices {
        render::reflection::Runtime::object_type_index,
        reflection::Probe::object_type_index,
        render::reflection::Probe::object_type_index
    };
    constexpr static std::array immediate_parent_object_type_indices {
        render::reflection::Runtime::object_type_index,
        reflection::Probe::object_type_index
    };

private:
    std::shared_ptr<texture::TextureCube> gapi_environment;
    std::shared_ptr<texture::TextureCube> gapi_irradiance;
    std::shared_ptr<texture::TextureCube> gapi_radiance;

    GapiCubeTarget gapi_environment_targets;
    GapiCubeTarget gapi_irradiance_targets;
    GapiMippedCubeTarget gapi_radiance_targets;

    vk::raii::DescriptorPool irradiance_descriptor_pool { nullptr };
    CubeDescriptorSet irradiance_descriptor_sets;

    vk::raii::DescriptorPool radiance_descriptor_pool { nullptr };
    MippedCubeDescriptorSet radiance_descriptor_sets;

    std::uint32_t environment_texture_index = static_cast<std::uint32_t>(-1);
    std::uint32_t environment_sampler_index = static_cast<std::uint32_t>(-1);

public:
    [[nodiscard]] const std::shared_ptr<texture::TextureCube>& get_gapi_environment() const { return gapi_environment; }
    [[nodiscard]] const std::shared_ptr<texture::TextureCube>& get_gapi_irradiance() const { return gapi_irradiance; }
    [[nodiscard]] const std::shared_ptr<texture::TextureCube>& get_gapi_radiance() const { return gapi_radiance; }

    [[nodiscard]] const GapiCubeTarget& get_gapi_environment_targets() const { return gapi_environment_targets; }
    [[nodiscard]] const GapiCubeTarget& get_gapi_irradiance_targets() const { return gapi_irradiance_targets; }
    [[nodiscard]] const GapiMippedCubeTarget& get_gapi_radiance_targets() const { return gapi_radiance_targets; }

    [[nodiscard]] vk::DescriptorPool get_irradiance_descriptor_pool() const { return *irradiance_descriptor_pool; }
    [[nodiscard]] const CubeDescriptorSet& get_irradiance_descriptor_sets() const { return irradiance_descriptor_sets; }

    [[nodiscard]] vk::DescriptorPool get_radiance_descriptor_pool() const { return *radiance_descriptor_pool; }
    [[nodiscard]] const MippedCubeDescriptorSet& get_radiance_descriptor_sets() const { return radiance_descriptor_sets; }

    [[nodiscard]] std::uint32_t get_environment_texture_index() const { return environment_texture_index; }
    [[nodiscard]] std::uint32_t get_environment_sampler_index() const { return environment_sampler_index; }

private:
    void initialise_gapi();

    Runtime(
        core::ecs::Entity* entity,
        const math::Aabb3<core::fp_t>& receive_box,
        const math::Aabb3<core::fp_t>& exclude_box,
        const math::Aabb3<core::fp_t>& include_box,
        std::string&& name);

public:
    static void construct(
        core::ecs::Entity* entity,
        const math::Aabb3<core::fp_t>& receive_box,
        const math::Aabb3<core::fp_t>& exclude_box,
        const math::Aabb3<core::fp_t>& include_box,
        std::string&& name,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCallerShared<Runtime>&& end_callback);
    ~Runtime() override;
    void convolute_irradiance(vk::CommandBuffer cmd) const;
    void convolute_radiance(vk::CommandBuffer cmd) const;
    void vk_update(vk::CommandBuffer cmd) const;
};
}
#endif